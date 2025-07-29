
/*
    Código de header para definição de funções necessárias para obter leituras do HX711, customizadas por
    Diogo Moraes, Diogo Cajueiro e João Neto

    --------------------------------UFPE 2025------------------------------------------------------------
*/
#ifndef LOADCELL_H
#define LOADCELL_H



//pinos PA10 -> RX
//      PA09 -> TX

//pinos de data e clock para hx711
#define DATAPIN_1 PB12
#define SCKPIN_1 PB13

#define DATAPIN_2 PB10
#define SCKPIN_2 PB11

#define DATAPIN_3 PB6 
#define SCKPIN_3 PB7

#define DATAPIN_4 PB4
#define SCKPIN_4 PB5

#define DATAPIN_5 PA7
#define SCKPIN_5 PB0

#define DATAPIN_6 PA3
#define SCKPIN_6 PA4


#define NUM_MEDICOES 25
#define NUM_CELS 1

#define SIGNAL_TIMEOUT 400 //ms
#define CALIB_TIMEOUT 2000 //ms

  
//define flags e variaveis auxiliares
extern volatile bool enData1;
extern bool* leituraValida;
extern uint32_t endConversionTime; 
typedef struct {

  uint32_t cel_carga_meas[NUM_CELS];
  uint32_t timestamp_ms[NUM_CELS]; 

} async_data;

extern volatile async_data HX711_data; 

//função para fazer extensão de sinal de 24 bits para 32 bits
int convert24bitstoLong(int value24bits);

//declara funções das interrupções

//levanta flag quando qualquer HX711 abaixar os pinos
//dataPin
void getDataRequest(void);


class Cel_Carga{

    private: 
      uint8_t _dataPin;
      uint8_t _sckPin;
      
  
    public:

        float tare;
        float offset;
        float scale; 
        int lastDoutLowTime; 
        uint8_t num_conversoes; 

        //método construtor
        Cel_Carga(uint8_t dataPin, uint8_t sckPin);

        // //método setar offset após calibração
        // void setOffset(float off);

        // //método setar scale após calibração
        // void setScale(float scl);

        // //método obter offset após calibração
        // float getOffset();
        // //método obter scale após calibração
        // float getScale();

        //método para retornar pino clock
        float getSckPin();
        //método para retornar pino de dados
        float getDataPin();
        //método obter valor digital a partir de HX711
        int readRaw(bool* leituraValida);

        //método para obter valor de força após conversão de valor digital
        float getValue();

        float doTare();
        float doCalibration(float massa_g);

        

  
  };

  #endif