
/*
    Código de header para definição de funções necessárias para obter leituras do HX711, customizadas por
    Diogo Moraes, Diogo Cajueiro e João Neto

    --------------------------------UFPE 2025------------------------------------------------------------
*/
#ifndef plataforma_h
#define plataforma_h



//pinos PA10 -> RX
//      PA09 -> TX

//pinos de data e clock para hx711
#define DATAPIN_1 PB12
#define SCKPIN_1 PB13

#define DATAPIN_2 PB10
#define SCKPIN_2 PB11

#define DATAPIN_3 2
#define SCKPIN_3 3

#define DATAPIN_4 4
#define SCKPIN_4 5

#define DATAPIN_5 6
#define SCKPIN_5 7

#define DATAPIN_6 8
#define SCKPIN_6 9

#define NUM_MEDICOES 25

#define SIGNAL_TIMEOUT 400 //ms
#define CALIB_TIMEOUT 2000 //ms

//função para fazer extensão de sinal de 24 bits para 32 bits
int convert24bitstoLong(int value24bits);


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
        int readRaw();

        //método para obter valor de força após conversão de valor digital
        float getValue();

        float doTare();
        float doCalibration(float massa_g);

        

  
  };

  #endif