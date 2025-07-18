
#include "Arduino.h"
#include "plataforma.h"

uint32_t testTime = 0;
uint32_t elapsedTest = 0; 

void getPeriodicSamples(void){

  
  //armazena delta_t em estrutra
  sync_data.delta_t = tempDelta_t; 

  //efetua conversão e mede tempo gasto
  uint32_t now = micros();
  sync_data.tensao = analogRead(ADC_VOLT_PIN);
  sync_data.corrente = analogRead(ADC_CURRENT_PIN);
  elapsed = now - tesTime; 
  testTime = now; 
  //atualiza largura de pulso
  updatePWMPulse();

  //sinaliza armazenamento dos dados síncronos
  syncDataReady = true; 
}

void onHallPulse(void){

  uint32_t now = micros();
  tempDelta_t = now - lastPulse; 
  lasPulse = now; 


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


void configSyncInterrupt(void){

 // === CONFIGURAÇÃO DO TIMER DE INTERRUPÇÃO COM TIM2 ===
  Update_Timer->setPrescaleFactor(72); // 1 MHz
  Update_Timer->setOverflow(T, MICROSEC_FORMAT); //5ms -> 200 Hz

  //define função a ser chamada a cada instante de amostragem
  Update_Timer->attachInterrupt(updatePWM);

  Update_Timer->resume();
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

void getDataRequest(void){

  //verifica os pinos que estão em estado lógico zero para identificar quais estão prontos
  //para transmitir
  if(!digitalRead(DATAPIN_1)){
    enData1 = true; 

  }
}

int convert24bitstoLong(int value24bits){

  value24bits  &= (0x00FFFFFF); //considera somente 24 bits menos significativos

  //se for valor negativo faz extensão de sinal
  if(value24bits & 0x00800000){ 

    //preenche bits mais signficativos para manter valor decimal
    return (value24bits |= 0xFF000000); 

  }
  else{
    return value24bits;
  }
}

Cel_Carga::Cel_Carga(uint8_t dataPin, uint8_t sckPin){

    _dataPin = dataPin;
    _sckPin= sckPin;

    pinMode(_dataPin, INPUT);
    pinMode(_sckPin, OUTPUT);
    
    offset = 0.0f;
    scale = 1.0f;

  }

  // void Cel_Carga::setOffset(float off) {
  // tareOffset = off;
  // }

  // void Cel_Carga::setScale(float scl) {
  // scale = scl;
  // }

  // float Cel_Carga::getOffset(){
  // return tareOffset;
  // }

  // float Cel_Carga::getScale(){
  //   return scale;
  // }

  float Cel_Carga::getSckPin(){
  return _sckPin;
  }

  float Cel_Carga::getDataPin() {
  return _dataPin;
  }

  int Cel_Carga::readRaw(){

      //seta contagem para 0
      int Count = 0;

      //Verifica se pino de dados está em 0 para indicar que está pronto para transmitir
      if(!digitalRead(_dataPin)){
        lastDoutLowTime = micros();
      
        //seta pino de clock para low para começar a puxar dados
        digitalWrite(_sckPin, LOW);
        delayMicroseconds(1);

        //envia 24 pulsos para retirar bit a bit
        for (char i=0; i < 24; i++){

            digitalWrite(_sckPin, HIGH);
            delayMicroseconds(1);
            Count = Count << 1;

            //incrementa contagem se datapin alto
            if(digitalRead(_dataPin)) 
                Count++; 

            digitalWrite(_sckPin, LOW);
            delayMicroseconds(1);
            
                
        }
        digitalWrite(_sckPin, HIGH);
        delayMicroseconds(1); 
        //Count=Count^0x800000;
        digitalWrite(_sckPin, LOW);
        num_conversoes++;
      }
      return(Count);

    }

    //função bloqueante que espera sinal estar pronto para retornar valor da tara
    float Cel_Carga::doTare(){

      int timeStartTare = millis();

      //espera 3 conversoes para estabilizar valor de tara 
      tare = (float)convert24bitstoLong(readRaw()); 

      Serial.print(" Palavra digital obtida em tara: ");
      Serial.print(tare);
      Serial.print(", tempo transcorrido na função doTare (ms): ");
      Serial.print(millis() - timeStartTare); 
      Serial.print(", Número de execução de tara: ");
      Serial.println(num_conversoes);

      return(tare);
       
    }

    float Cel_Carga::doCalibration(float massa_g){
      int timeStartCal = millis();

        //acha coeficientes da linha m(p) = scale*p + offset onde 'p' é a palavra digital e 'm' a massa
        //com os pontos (p1, 0) e (p2, m1) 

        scale = massa_g / ((float)convert24bitstoLong(readRaw()) - tare);
        offset = -scale*tare;  

        Serial.print("Valor de escala obtido em calibração: ");
        Serial.print(scale, 8);
        Serial.print(", Valor de offset obtido em calibração: ");
        Serial.print(offset, 8);
        Serial.print(", tempo transcorrido na função doCalibration (ms): ");
        Serial.print(millis() - timeStartCal); 
        Serial.print(", Número de execução de caliibração: ");
        Serial.println(num_conversoes);

      return(scale);

    }

    float Cel_Carga::getValue() {

      return((float)convert24bitstoLong(readRaw())*scale + offset);
    }