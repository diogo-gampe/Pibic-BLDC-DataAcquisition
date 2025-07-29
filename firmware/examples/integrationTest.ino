#include <Arduino.h>
#include "loadCell.h"
#include "peripherals.h"

//define celulas de carga
Cel_Carga celula1(DATAPIN_1, SCKPIN_1); 

//define flags e variaveis auxiliares
uint32_t startTime =0; 
uint8_t forceCount = 0; 
uint32_t endAcquireTime = 0;
uint32_t meas_HX711 = 0; 

void readAsync_ifReady(void){
    //verifica quais dados estão prontos para começar o processo de transferência de dados
    if(enData1){

      //armazena valor digital bruto com extensão de sinal no vetor medicoes[]
      uint32_t now = micros();
      HX711_data.cel_carga_meas[0] = celula1.getValue();
      meas_HX711 = micros() - now; 
      //armazena tempo de obtenção do dado
      HX711_data.timestamp_ms[0] = millis() - startTime; 
      
      //incrementa variável para indicar o número de células que já armazenaram uma medição
      //forceCount++;

      //limpa flag que sinaliza prontidão dos dados
      enData1 = false;


      for(uint8_t i = 0; i < NUM_CELS; i++){

        Serial.printf("HX711[%d] ==> Tempo Gasto Total (us): %d | Tempo gasto c/ pulsos de clock: %d | Leitura Interrompida?: %d |  Medida: %d | Tempo de aquisição (ms): %d\n",
        i, 
        meas_HX711,
        endConversionTime,
        !(*leituraValida),
        HX711_data.cel_carga_meas[i], 
        HX711_data.timestamp_ms[i]);
    }
      
    }
}

void setup(){

//inicializa Serial
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(HALL_PIN, INPUT);


  Serial.print("Para iniciar processo de medição aperte qualquer tecla: "); 
  while(!Serial.available());
  Serial.println("Iniciando processo de medição...");


  attachInterrupt(digitalPinToInterrupt(DATAPIN_1), getDataRequest, FALLING);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), onHallPulse, RISING);

  configPWM();
  configADC();
  configDMA();
  configSyncInterrupt();

  startTime = millis();

}



void loop() {
  
  if(syncDataReady){

    syncDataReady = false;    
    readAsync_ifReady();

    Serial.printf("Valores síncronos ==> Tempo na ISR Sync: %d | Validade dos dados: %d | ADC da corrente: %d | ADC da Tensão: %d | Medida Hall (us): %d\n",
        elapsedTest_Sync,
        !ISR_data.erro, 
        ISR_data.corrente,
        ISR_data.tensao,
        ISR_data.delta_t 
    );
  }



}
