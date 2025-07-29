
#include "Arduino.h"
#include "loadCell.h"
#include "peripherals.h"

volatile bool enData1 = false;
bool leituraValidaValor = true;
bool* leituraValida = &leituraValidaValor;

volatile async_data HX711_data = { 0 };

uint32_t endConversionTime = 0; 

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

  int Cel_Carga::readRaw(bool* leituraValida){

      //seta contagem para 0
      int Count = 0;
      *leituraValida = true; 

      //Verifica se pino de dados está em 0 para indicar que está pronto para transmitir
      if(!digitalRead(_dataPin)){
        
        uint32_t t0 = isrSyncCounter; 
        uint32_t startConversionTime = micros();
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

        //ao fim dos pulsos verifica se pulsos de clock foram interrompidos por outra ISR
        uint32_t t1 = isrSyncCounter; 

        //caso sim retorna leitura invalida
        *leituraValida = (t1 != t0) ? false : true; 
        
        endConversionTime = micros() - startConversionTime; 
      }
      return(Count);

    }

    //função bloqueante que espera sinal estar pronto para retornar valor da tara
    float Cel_Carga::doTare(){

      int timeStartTare = millis();

      //espera 3 conversoes para estabilizar valor de tara 
      tare = (float)convert24bitstoLong(readRaw(leituraValida)); 

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

        scale = massa_g / ((float)convert24bitstoLong(readRaw(leituraValida)) - tare);
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

      return((float)convert24bitstoLong(readRaw(leituraValida))*scale + offset);
    }