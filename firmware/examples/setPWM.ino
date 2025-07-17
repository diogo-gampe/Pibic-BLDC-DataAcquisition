#include <Arduino.h>
#define PWM_PIN PB4
#define HALL_PIN PB8

#define PWM_FREQUENCY 200 // 5ms de período
#define PWM_PERIOD (1000000/PWM_FREQUENCY) //Periodo em microsegundos

#define MIN_PULSE_US 0 //mínima largura de pulso para ESC 1ms
#define MAX_PULSE_US PWM_PERIOD //máxima largura de pulso para ESC 2ms

#define T PWM_PERIOD //período de amostragem em microsegundos

//inicializa largura de pulso como 1 ms
uint32_t pulse_us = MIN_PULSE_US ;
bool subindo = true; 
uint32_t start_ms = 0; 


//descobre canal e Timer associado ao pino 
// usado para ser compatível com qualquer STM32 e é mais prático
uint32_t pwm_channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PWM_PIN), PinMap_PWM));
TIM_TypeDef *PWM_Instance = (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(PWM_PIN), PinMap_PWM);
HardwareTimer *PWM_Timer = new HardwareTimer(PWM_Instance);


HardwareTimer *Update_Timer = new HardwareTimer(TIM2);

float multiseno_us(uint32_t t_us, float n) {
  float u = 0;
  float tt = (float)t_us / 1e5; // tempo em 0.01s

  for(int8_t j = 0; j < n; j++) {
    u += cos((W_MIN + (j * (W_MAX - W_MIN) / n)) * tt - (j * (j + 1) * PI) / n);
  }

  //pedaço comenetado estava na função de Matheus
  u = GAIN * u;
  u = OFFSET + u;

  //estou utilizando essa versão
  //essa versão está errada devido ao mapeamento de -1 até 1, na verdade a variação é diferente
  u = map(u, -1, 1, MIN_PULSE_US, MAX_PULSE_US);
  return u; //
}

void updatePWM() {
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



void pwmConfig(){

  PWM_Timer->setPrescaleFactor(72); // 72 MHz / 72 = 1 MHz (1 us por tick)
  PWM_Timer->setOverflow(PWM_PERIOD, MICROSEC_FORMAT); // 200 Hz → 5 ms

  //seta modo do PWM ==> TIM_OCMODE_PWM1            pino alto quando counter < channel , baixo c.c
  PWM_Timer->setMode(pwm_channel, TIMER_OUTPUT_COMPARE_PWM1, PWM_PIN);

  //seta valor da comparação nos registradores a partir de duração
  PWM_Timer->setCaptureCompare(pwm_channel, MIN_PULSE_US, MICROSEC_COMPARE_FORMAT);
  

  PWM_Timer->resume();

}

void interruptConfig(){

 // === CONFIGURAÇÃO DO TIMER DE INTERRUPÇÃO COM TIM2 ===
  Update_Timer->setPrescaleFactor(72); // 1 MHz
  Update_Timer->setOverflow(T, MICROSEC_FORMAT); //5ms -> 200 Hz

  //define função a ser chamada a cada instante de amostragem
  Update_Timer->attachInterrupt(updatePWM);

  Update_Timer->resume();
}

void setup()
{

  start_ms = millis();
  Serial.begin(115200);
  

  pwmConfig();
  

  while(millis() <= start_ms + 3000){
    PWM_Timer->setCaptureCompare(pwm_channel, MAX_PULSE_US, MICROSEC_COMPARE_FORMAT);
  }

  interruptConfig();
  
}


void loop()
{
  
  uint32_t now = millis();

  if(now >= start_ms + 500){
    
    Serial.printf("Largura de pulso (us): %d\n",pulse_us);
  }  
  
}