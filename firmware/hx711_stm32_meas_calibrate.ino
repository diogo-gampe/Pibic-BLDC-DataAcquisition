#include <Arduino.h>


#define DATAPIN PB12
#define SCKPIN PB13


#define NUM_MEDICOES 5

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


signed long vec_meas[10];
uint8_t serial_byte;  
uint8_t i = 0; 
bool resume = false; 
bool flag_leitura = false; 

int pesos[NUM_MEDICOES] = {0, 100, 200, 400, 500}; //pesos de calibração em gramas
signed long medicoes[NUM_MEDICOES] = {0, 0, 0, 0, 0}; //medições obtidas

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATAPIN, INPUT);
  pinMode(SCKPIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Iniciando processo de medição...");
}

// the loop function runs over and over again forever
void loop() {
 
  
  Serial.print("Insira peso de: ");
  Serial.print(pesos[i]);
  Serial.println(" gramas, aperte 'S' para confirmar...");

  while(!Serial.available()){

  }

  if(Serial.read() == 'S'){
    
    medicoes[i] = digitalRead(convert24bitstoLong(Read_Raw24bits(DATAPIN, SCKPIN)));

    Serial.print("Leitura armazenada: ");
    Serial.println(medicoes[i], BIN);
    i++; 

  }

  if(i >= NUM_MEDICOES){

    i = 0;
    Serial.println("Medicoes Obtidas: ");
    for(int j = 0; j <= NUM_MEDICOES; j++){
      Serial.print(pesos[j]);
      Serial.print(" gramas || ");
      Serial.print(medicoes[j], BIN);
      Serial.println(" valor digital");
    }
  }
  //Serial.println(convert24bitstoLong(Read_Raw24bits(DATAPIN, SCKPIN)), DEC);

  // digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  // delay(500);                      // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  // delay(500);                      // wait for a second
 
}
