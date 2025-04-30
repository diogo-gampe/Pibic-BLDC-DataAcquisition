
#include "Arduino.h"
#include "plataforma.h"

signed long convert24bitstoLong(signed long value24bits){

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

  void Cel_Carga::setOffset(float off) {
  offset = off;
  }

  void Cel_Carga::setScale(float scl) {
  scale = scl;
  }

  float Cel_Carga::getOffset(){
  return offset;
  }

  float Cel_Carga::getScale(){
    return scale;
  }

  float Cel_Carga::getSckPin(){
  return _sckPin;
  }

  float Cel_Carga::getDataPin() {
  return _dataPin;
  }

  signed long Cel_Carga::readRaw(){

      //seta contagem para 0
      signed long Count = 0;

      //Verifica se pino de dados está em 0 para indicar que está pronto para transmitir
      if(!digitalRead(_dataPin)){
      
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
      Count=Count^0x800000;
      digitalWrite(_sckPin, LOW);
      }

      return(Count);

    }

    float Cel_Carga::getValue() {
      signed long raw = readRaw();
      return (convert24bitstoLong(raw) - offset) / scale;
    }