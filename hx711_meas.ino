#include <Arduino.h>


#define DATAPIN_1 PB12
#define SCKPIN_1 PB13

#define DATAPIN_2
#define SCKPIN_2

#define DATAPIN_3
#define SCKPIN_3

#define DATAPIN_4
#define SCKPIN_4

#define DATAPIN_5
#define SCKPIN_5

#define DATAPIN_6
#define SCKPIN_6

#define NUM_MEDICOES 6



uint8_t i = 0; 

int pesos[NUM_MEDICOES] = {0, 100, 200, 400, 500}; //pesos de calibração em gramas
signed long medicoes[NUM_MEDICOES] = {0, 0, 0, 0, 0}; //medições obtidas

bool enData1, enData2, enData3, enData4, enData5, enDat6;

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

signed long Read_Raw24bits(uint8_t datapin, uint8_t sckpin){
    //seta contagem para 0
    signed long Count = 0;

    //Verifica se pino de dados está em 0 para indicar que está pronto para transmitir
    if(!digitalRead(datapin)){
    
    //seta pino de clock para low para começar a puxar dados
    digitalWrite(sckpin, LOW);
    delayMicroseconds(1);

    //envia 24 pulsos para retirar bit a bit
    for (char i=0; i < 24; i++){

        digitalWrite(sckpin, HIGH);
        delayMicroseconds(1);
        Count = Count << 1;

        //incrementa contagem se datapin alto
        if(digitalRead(datapin)) 
            Count++; 

        digitalWrite(sckpin, LOW);
        delayMicroseconds(1);
        
            
    }
    digitalWrite(sckpin, HIGH);
    delayMicroseconds(1); 
    Count=Count^0x800000;
    digitalWrite(sckpin, LOW);
    }

    return(Count);

}

void getDataRequest(void){

//se qu
  if(!DATAPIN_1){
    enData1 = true; 
  }
   if(!DATAPIN_2){
    enData2 = true; 
   }
 if(!DATAPIN_3){
    enData3 = true; 
 }
 if(!DATAPIN_4){
    enData4 = true; 
 }

 if(!DATAPIN_5){
    enData5 = true; 
 }
 if(!DATAPIN_6){
    enData6 = true; 
 }


}


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATAPIN, INPUT);
  pinMode(SCKPIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest(), FALLING);
  attachInterrupt(digitalPinToInterrupt(DATAPIN_2), getDataRequest(), FALLING);
  attachInterrupt(digitalPinToInterrupt(DATAPIN_3), getDataRequest(), FALLING);
  attachInterrupt(digitalPinToInterrupt(DATAPIN_4), getDataRequest(), FALLING);
  attachInterrupt(digitalPinToInterrupt(DATAPIN_5), getDataRequest(), FALLING);
  attachInterrupt(digitalPinToInterrupt(DATAPIN_6), getDataRequest(), FALLING);


  Serial.begin(115200);
  Serial.println("Iniciando processo de medição...");
}

// the loop function runs over and over again forever
void loop() {
 
  if(enData1){
    medicoes[0] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN_1, SCKPIN_1)));
    i++;
  }

  
  if(enData2){
    medicoes[1] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN_2, SCKPIN_2)));
    i++;
  }
 
 
  if(enData3){
    medicoes[2] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN_3, SCKPIN_3)));
    i++;
  }
 
  if(enData4){
    medicoes[3]] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN_4, SCKPIN_4)));
    i++;
  }
  
  
  if(enData5){
    medicoes[4] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN_5, SCKPIN_5)));
    i++;
  }
 
  if(enData6){
    medicoes[5] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN_6, SCKPIN_6)));
    i++;
  }

  if(i >= NUM_MEDICOES - 1){

    detachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest(), FALLING);
    detachInterrupt(digitalPinToInterrupt(DATAPIN_2), getDataRequest(), FALLING);
    detachInterrupt(digitalPinToInterrupt(DATAPIN_3), getDataRequest(), FALLING);
    detachInterrupt(digitalPinToInterrupt(DATAPIN_4), getDataRequest(), FALLING);
    detachInterrupt(digitalPinToInterrupt(DATAPIN_5), getDataRequest(), FALLING);
    detachInterrupt(digitalPinToInterrupt(DATAPIN_6), getDataRequest(), FALLING);

    for(uint8_t j = 0; j < NUM_MEDICOES - 1; j++){
      Serialprintln(medicoes[j]);
    }

    enData1 = false;
    enData2 = false;
    enData3 = false;
    enData4 = false;
    enData5 = false;
    enData6 = false;

    attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest(), FALLING);
    attachInterrupt(digitalPinToInterrupt(DATAPIN_2), getDataRequest(), FALLING);
    attachInterrupt(digitalPinToInterrupt(DATAPIN_3), getDataRequest(), FALLING);
    attachInterrupt(digitalPinToInterrupt(DATAPIN_4), getDataRequest(), FALLING);
    attachInterrupt(digitalPinToInterrupt(DATAPIN_5), getDataRequest(), FALLING);
    attachInterrupt(digitalPinToInterrupt(DATAPIN_6), getDataRequest(), FALLING);
  }

 
}
