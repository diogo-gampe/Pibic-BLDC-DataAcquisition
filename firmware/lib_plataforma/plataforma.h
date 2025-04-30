
/*
    Código de header para definição de funções necessárias para obter leituras do HX711, customizadas por
    Diogo Moraes, Diogo Cajueiro e João Neto

    --------------------------------UFPE 2025------------------------------------------------------------
*/
#ifndef plataforma_h
#define plataforma_h


#define DATAPIN_1 PB12
#define SCKPIN_1 PB13

#define DATAPIN_2 0
#define SCKPIN_2 1

#define DATAPIN_3 2
#define SCKPIN_3 3

#define DATAPIN_4 4
#define SCKPIN_4 5

#define DATAPIN_5 6
#define SCKPIN_5 7

#define DATAPIN_6 8
#define SCKPIN_6 9

#define NUM_MEDICOES 5

//função para fazer extensão de sinal de 24 bits para 32 bits
signed long convert24bitstoLong(signed long value24bits);


class Cel_Carga{

    private: 
      uint8_t _dataPin;
      uint8_t _sckPin;
  
      float offset;
      float scale; 
    
    public:
        //método construtor
        Cel_Carga(uint8_t dataPin, uint8_t sckPin);

        //método setar offset após calibração
        void setOffset(float off);

        //método setar scale após calibração
        void setScale(float scl);

        //método obter offset após calibração
        float getOffset();
        //método obter scale após calibração
        float getScale();

        //método para retornar pino clock
        float getSckPin();
        //método para retornar pino de dados
        float getDataPin();
        //método obter valor digital a partir de HX711
        signed long readRaw();

        //método para obter valor de força após conversão de valor digital
        float getValue();
  
  };

  #endif