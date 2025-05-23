#include <Arduino.h>
#include "plataforma.h"
#include <EEPROM.h>


#define CEL_CARGA_INFO_ADDRESS 0x00
#define CALIBRATION_MASS 400 //iniciamos com 400g para calibração (valor arbitrário)

//variável para contar medições obtidas
uint8_t i = 0;
//vetor para armazenar medições de 6 células de carga
int medicoes[NUM_MEDICOES] = {0, 0};
int tempoCorrido[NUM_MEDICOES] = {0, 0};

//variáveis para indicar que o hx711 indicou que está pronto para transmitir o dado
volatile bool enData1 = false;
volatile bool enData2 = false;

int startTime; 

// bool enData3 = false;
// bool enData4 = false;
// bool enData5 = false;
// bool enData6 = false;

//declarando objeto de celula de carga com seus respectivos pinos de dado e clock
//função do construtor já declara pinos como saída/entrada
Cel_Carga celula1(DATAPIN_1, SCKPIN_1); 
Cel_Carga celula2(DATAPIN_2, SCKPIN_2); 
// Cel_Carga celula3(DATAPIN_3, SCKPIN_3); 
// Cel_Carga celula4(DATAPIN_4, SCKPIN_4); 
// Cel_Carga celula5(DATAPIN_5, SCKPIN_5);
// Cel_Carga celula6(DATAPIN_6, SCKPIN_6); 

//função para ser executada na interrupção
void getDataRequest(void){

  //verifica os pinos que estão em estado lógico zero para identificar quais estão prontos
  //para transmitir
  if(!digitalRead(DATAPIN_1)){
    enData1 = true; 

  }
  
  if(!digitalRead(DATAPIN_2)){
    enData2 = true; 
  }
//  if(!digitalRead(DATAPIN_3)){
//     enData3 = true; 
//  }
//  if(!digitalRead(DATAPIN_4)){
//     enData4 = true; 
//  }

//  if(!digitalRead(DATAPIN_5)){
//     enData5 = true; 
//  }
//  if(!digitalRead(DATAPIN_6)){
//     enData6 = true; 
//  }
 
 return; 
}

// the setup function runs once when you press reset or power the board
void setup() {

  //inicializa led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  //declara pinos de interrupção na borda de descida, para chamar função getDataRequest
  attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest, FALLING);
  attachInterrupt(digitalPinToInterrupt(DATAPIN_2), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_3), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_4), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_5), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_6), getDataRequest, FALLING);

  //inicializa Serial
  Serial.begin(115200);

  Serial.print("Para iniciar processo de medição aperte qualquer tecla: "); 
  while(!Serial.available());
  Serial.println("Iniciando processo de medição...");

  startTime = micros();

}

// the loop function runs over and over again forever
void loop() {
  
  //verifica quais dados estão prontos para começar o processo de transferência de dados
  if(enData1){

    
    //armazena tempo de obtenção do dado
    tempoCorrido[0] = micros() - startTime; 
    //armazena valor digital bruto com extensão de sinal no vetor medicoes[]
    medicoes[0] = celula1.getValue();

    //ao armazenar a medição desabilita interrupção do pino
    //aguarda obter uma medição de cada celula de carga para reestabelecer as interrupções
    detachInterrupt(digitalPinToInterrupt(DATAPIN_1));

    //incrementa variável para indicar o número de células que já armazenaram uma medição
    i++;
  }

  
  if(enData2){
    //armazena tempo corrido entre quedas do sinal dout
    tempoCorrido[1] = micros() - startTime; 

    medicoes[1] = celula2.getValue();
    detachInterrupt(digitalPinToInterrupt(DATAPIN_2));
    i++;
  }
 
 
  // if(enData3){
  //   medicoes[2] = convert24bitstoLong(celula3.readRaw());
  //   detachInterrupt(digitalPinToInterrupt(DATAPIN_3));
  //   i++;
  // }
 
  // if(enData4){
  //   medicoes[3] = convert24bitstoLong(celula4.readRaw());
  //   detachInterrupt(digitalPinToInterrupt(DATAPIN_4));
  //   i++;
  // }
  
  
  // if(enData5){
  //   medicoes[4] = convert24bitstoLong(celula5.readRaw());
  //   detachInterrupt(digitalPinToInterrupt(DATAPIN_5));
  //   i++;
  // }
 
  // if(enData6){
  //   medicoes[5] = convert24bitstoLong(celula6.readRaw());
  //   detachInterrupt(digitalPinToInterrupt(DATAPIN_6));
  //   i++;
  // }
  

  //se forem efetuadas 6 medições transmite vetor por serial, reestabelece as interrupções e reseta variáveis
  if(i >= NUM_MEDICOES - 1){


    for(uint8_t j = 0; j <= NUM_MEDICOES - 1; j++){
      Serial.print("Celula ");
      Serial.print(j+1);
      Serial.print(": ");
      Serial.print(medicoes[j]);
      Serial.print(", tempo (us): ");
      Serial.println(tempoCorrido[j]);
    }
    
    enData1 = false;
    enData2 = false;
    // enData3 = false;
    // enData4 = false;
    // enData5 = false;
    // enData6 = false;

    attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest, FALLING);
    attachInterrupt(digitalPinToInterrupt(DATAPIN_2), getDataRequest, FALLING);
    // attachInterrupt(digitalPinToInterrupt(DATAPIN_3), getDataRequest, FALLING);
    // attachInterrupt(digitalPinToInterrupt(DATAPIN_4), getDataRequest, FALLING);
    // attachInterrupt(digitalPinToInterrupt(DATAPIN_5), getDataRequest, FALLING);
    // attachInterrupt(digitalPinToInterrupt(DATAPIN_6), getDataRequest, FALLING);

    i=0;
  }
}
