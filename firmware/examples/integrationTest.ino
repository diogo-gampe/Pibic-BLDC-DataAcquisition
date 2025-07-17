
#include <Arduino.h>
#include "plataforma.h"

//define celulas de carga
Cel_Carga celula1(DATAPIN_1, SCKPIN_1); 

//define flags e variaveis auxiliares
volatile bool syncDataReady = false;
uint8_t forceCount = 0; 



void setup(){

    pinMode(LED_BUILTIN, OUPUT);
    attachInterrupt(digitalPinToInterrupt(celula1.getDataPin()), getDataRequest, FALLING);


//inicializa Serial
  Serial.begin(115200);

  Serial.print("Para iniciar processo de medição aperte qualquer tecla: "); 
  while(!Serial.available());
  Serial.println("Iniciando processo de medição...");

  startTime = millis();

}



void loop() {
  
  //verifica quais dados estão prontos para começar o processo de transferência de dados
  if(enData1){

    
    //armazena tempo de obtenção do dado
    async_data.timestamp_ms[0] = micros() - startTime; 
    //armazena valor digital bruto com extensão de sinal no vetor medicoes[]
    async_data.cel_carga_meas[0] = celula1.getValue();

    //ao armazenar a medição desabilita interrupção do pino
    //aguarda obter uma medição de cada celula de carga para reestabelecer as interrupções
    detachInterrupt(digitalPinToInterrupt(celula1.getDataPin()));

    //incrementa variável para indicar o número de células que já armazenaram uma medição
    forceCount++;

    //limpa flag que sinaliza prontidão dos dados
    enData1 = false
  }

  if(syncDataReady){

    for(uint8_t j = 0; j < NUM_CELS; j++){

        Serial.printf("HX711[%d] ==> ID: %d | Medida: %d | Tempo de aquisição (ms): %d\n",
        i, 
        i, 
        async_data.cel_carga_meas[i], 
        async_data.timestamp_ms[i]);
    }

    Serial.printf("Valores síncronos ==> ADC da corrente: %d | ADC da Tensão: %d | Medida Hall (us): %d\n",
    
        sync_data.corrente,
        sync_data.tensao,
        sync_data.delta_t
    );

    syncDataReady = false; 

  }

}
