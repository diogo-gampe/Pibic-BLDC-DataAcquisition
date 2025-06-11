#include <Arduino.h>
#include "plataforma.h"


// esse código servirá para obter as palavras digitais geradas pelo HX711 quando estimuladas
// por um mesmo peso afim de obter uma distribuição probabilistica para a medição

#define NUM_PESOS 3


//variável para contar medições obtidas
uint16_t count_medicoes = 0;
uint8_t count_pesos = 0;

//vetor para armazenar medições de uma única célula de carga com 
int medicoes[NUM_PESOS][NUM_MEDICOES];
int pesos[NUM_PESOS]; 

//int tempoCorrido[NUM_MEDICOES];

//variáveis para indicar que o hx711 indicou que está pronto para transmitir o dado
volatile bool enData1 = false;
volatile bool enData2 = false;
// volatile bool enData3 = false;
// volatile bool enData4 = false;
// volatile bool enData5 = false;
// volatile bool enData6 = false;

bool resume = true; 
bool done = true;

int startTime; 



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
  //attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest, FALLING);
  //attachInterrupt(digitalPinToInterrupt(DATAPIN_2), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_3), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_4), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_5), getDataRequest, FALLING);
  // attachInterrupt(digitalPinToInterrupt(DATAPIN_6), getDataRequest, FALLING);

  //inicializa Serial
  Serial.begin(115200);

  
  while(!Serial.available());
  startTime = micros();

}

// the loop function runs over and over again forever
void loop() {
  
    if(resume){
        //recebe valor do peso que será colocado na balança
        Serial.print("Digite o valor do peso que deseja obter as medições: ");
        while(!Serial.available());

        peso = Serial.parseInt();
        count_peso++; 

        attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest, FALLING);

        resume = false;
    }

  //verifica quais dados estão prontos para começar o processo de transferência de dados
  if(enData1){


    //armazena valor digital bruto com extensão de sinal no vetor medicoes[]
    medicoes[count_peso - 1][count_medicoes] = celula1.getValue();

    //ao armazenar a medição desabilita interrupção do pino
    //aguarda obter uma medição de cada celula de carga para reestabelecer as interrupções
    //detachInterrupt(digitalPinToInterrupt(DATAPIN_1));

    //incrementa variável para indicar o número de células que já armazenaram uma medição
    count_medicoes++;

    if(count_medicoes >= NUM_MEDICOES - 1){
        resume = true; 
        count_medicoes = 0;
        if(conta_peso >= NUM_PESOS)
            done = true;
    }
  }

  
//   if(enData2){
//     //armazena tempo corrido entre quedas do sinal dout
//     tempoCorrido[1] = micros() - startTime; 

//     medicoes[1] = celula2.getValue();
//     detachInterrupt(digitalPinToInterrupt(DATAPIN_2));
//     i++;
//   }
 
 
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
  if(done){

    for(int i = 0; i <= NUM_PESOS - 1; i++){
        Serial.println(peso[i]);
        for(uint8_t j = 0; j <= NUM_MEDICOES - 1; j++){
        
        Serial.print(medicoes[i][j]);;
        Serial.print(", ");

        }        
    }
    
    enData1 = false;
    //enData2 = false;
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
