

#include "Arduino.h"
#include "peripherals.h"
#include "loadCell.h"


//descobre canal e Timer associado ao pino 
// usado para ser compatível com qualquer STM32 e é mais prático
uint32_t pwm_channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PWM_PIN), PinMap_PWM));

//com pino PB1 o timer associado é TIMER1
TIM_TypeDef *PWM_Instance = (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(PWM_PIN), PinMap_PWM);

//declara objeto de timer associado ao PWM
//nesse caso que PB1 é o pino do PWM Timer1 é o timer associado
//se precisar mudar de pino PWM é preciso garantir que o pino associado não será o timer da interrupção períodica
HardwareTimer *PWM_Timer = new HardwareTimer(PWM_Instance);
HardwareTimer *Update_Timer = new HardwareTimer(TIM3);

//contador de tempo para ISR períodica
uint32_t elapsedTest_Sync = 0; 

//armazena temporariamente intervalo de tempo entre
//pulso do sensor Hall 
volatile uint32_t tempDelta_t = 0;  

//flag para indicar fim de ISR periódica
volatile bool syncDataReady = false;

//contagem de número de ISRs executadas para debug
volatile uint32_t isrSyncCounter = 0;

//endereço para DMA
volatile uint16_t adcBuffer[2];

//variavel para armazaenar largura do pulso pwm
//static garante única definição para ser usada no escopo desse arquivo cpp
uint32_t pulse_us = MAX_PULSE_US;

//variavel para atualizar PWM
bool subindo = true; 

//variavel para onHallPulse()
uint32_t lastPulse;

//inicializa variavel do tipo sync_data
volatile sync_data ISR_data = {
  .corrente = 0,
  .tensao = 0,
  .delta_t = 0,
  .erro = false
};


void onHallPulse(void){
  uint32_t now = micros();
  tempDelta_t = now - lastPulse; 
  lastPulse = now; 
}

void updatePWMPulse() {
  //Atualiza valor do pulso entre 1000 us e 2000 us
  if (subindo) {
    pulse_us += 10;
    if (pulse_us >= MAX_PULSE_US) subindo = false;
  } else {
    pulse_us -= 10;
    if (pulse_us <= MIN_PULSE_US) subindo = true;
  }

  PWM_Timer->setCaptureCompare(pwm_channel, pulse_us, MICROSEC_COMPARE_FORMAT);
}

void getPeriodicSamples(void){

  uint32_t now_Sync = micros(); 

  //armazena delta_t em estrutra
  ISR_data.delta_t = tempDelta_t; 

  if(DMA1->ISR & DMA_ISR_TEIF1){
    ISR_data.erro = true;
    // Limpa flags de erro
    DMA1->IFCR |= DMA_IFCR_CTEIF1 | DMA_IFCR_CTCIF1;

    // (Opcional) Reinicia o canal DMA se necessário
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMA1_Channel1->CNDTR = 2;
    DMA1_Channel1->CCR |= DMA_CCR_EN;

  }
  else{
    if(DMA1->ISR & DMA_ISR_TCIF1){

    //limpa flag; 
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
    ISR_data.tensao = adcBuffer[1];
    ISR_data.corrente = adcBuffer[0];
    ISR_data.erro = false; 
    }
    else{
      ISR_data.erro = true; 
    }
  }
 

  //atualiza largura de pulso
  updatePWMPulse();
  //sinaliza armazenamento dos dados síncronos
  syncDataReady = true; 
  elapsedTest_Sync = micros() - now_Sync; 
}


void configDMA(void){
  // 1. Habilita clock do DMA
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  // 2. Desabilita canal antes de configurar
  DMA1_Channel1->CCR &= ~DMA_CCR_EN;


  DMA1_Channel1->CCR = 0;  // reseta canal

  DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;         // origem: DR
  DMA1_Channel1->CMAR = (uint32_t)&adcBuffer[0];    // destino: buffer
  DMA1_Channel1->CNDTR = 2;                          // número de transferências

  // 5. Configura CCR
  DMA1_Channel1->CCR =
      DMA_CCR_MINC        |  // incrementa endereço da memória em 2 bytes
      DMA_CCR_MSIZE_0     |  // memória de 16 bits (01)
      DMA_CCR_PSIZE_0     |  // periférico de 16 bits (01)
      DMA_CCR_CIRC        |  // modo circular
      DMA_CCR_PL_1;          // prioridade alta (10) - opcional

  // 6. Habilita o canal
  DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void configADC(void){

  // Habilita clock para GPIOA e ADC1
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

  ADC1->CR2 = 0;
  ADC1->CR1 = 0;

   // 2. GPIOs analógicos
  GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);  // PA0
  GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);  // PA1

  ADC1->CR1 = ADC_CR1_SCAN;

  ADC1->SQR1 = ADC_SQR1_L_0;          // 2 canais      
  ADC1->SMPR2 |= (4 << ADC_SMPR2_SMP0_Pos) | (4 << ADC_SMPR2_SMP1_Pos); // 55.5 ciclos
  ADC1->SQR3 = (1 << 5) | 0;  // PA1 primeiro, PA0 depois   

  ADC1->CR2 |= (0b100 << ADC_CR2_EXTSEL_Pos); // trigger externo = TIM3 TRGO
  ADC1->CR2 |= ADC_CR2_EXTTRIG;               // habilita trigger externo

  ADC1->CR2 |= ADC_CR2_ADON;
  delay(1);
  ADC1->CR2 |= ADC_CR2_RSTCAL;
  while (ADC1->CR2 & ADC_CR2_RSTCAL);
  ADC1->CR2 |= ADC_CR2_CAL;
  while (ADC1->CR2 & ADC_CR2_CAL);

  ADC1->CR2 |= ADC_CR2_DMA;

}

void configSyncInterrupt(void){

 // Habilita clock do Timer 3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // Timer 3: 72 MHz / (71+1) = 1 MHz → overflow a cada 1024 µs
  TIM3->PSC = 71;              // prescaler para 1 MHz
  TIM3->ARR = 1024 - 1;        // overflow a cada 1.024 ms

  //Configura TRGO para "Update Event"
  //para ADC
  TIM3->CR2 &= ~TIM_CR2_MMS;
  TIM3->CR2 |= (0b010 << TIM_CR2_MMS_Pos);  // MMS = 010 → Update event como TRGO

  //define função a ser chamada a cada instante de amostragem
  Update_Timer->attachInterrupt(getPeriodicSamples);

    // Gera evento de atualização imediatamente (para ativar TRGO corretamente)
  TIM3->EGR |= TIM_EGR_UG;
  TIM3->CR1 |= TIM_CR1_CEN;    // inicia o Timer
}


void configPWM(void){

  PWM_Timer->setPrescaleFactor(72); // 72 MHz / 72 = 1 MHz (1 us por tick)
  PWM_Timer->setOverflow(PWM_PERIOD, MICROSEC_FORMAT); // 200 Hz → 5 ms

  //seta modo do PWM ==> TIM_OCMODE_PWM1            pino alto quando counter < channel , baixo c.c
  PWM_Timer->setMode(pwm_channel, TIMER_OUTPUT_COMPARE_PWM1, PWM_PIN);

  //seta valor da comparação nos registradores a partir de duração
  PWM_Timer->setCaptureCompare(pwm_channel, MIN_PULSE_US, MICROSEC_COMPARE_FORMAT);
  
  PWM_Timer->resume();

}

void Interrupts(void){
  attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest, FALLING);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), onHallPulse, FALLING);
  Update_Timer->attachInterrupt(getPeriodicSamples);
}

void disableInterrupts(void){
  detachInterrupt(digitalPinToInterrupt(DATAPIN_1));
  detachInterrupt(digitalPinToInterrupt(HALL_PIN));
  Update_Timer->detachInterrupt();
}

