
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

#define HALL_PIN PB8
#define PWM_PIN PB4
#define ADC_CURRENT_PIN PA0
#define ADC_VOLT_PIN PA1


#define NUM_MEDICOES 25
#define NUM_CELS 6

#define SIGNAL_TIMEOUT 400 //ms
#define CALIB_TIMEOUT 2000 //ms

//T -> período de amostragem
#define T 1024 //us

//declara objeto de timer com Timer2
//gerará interrupções períodicas para amostragem
HardwareTimer *Update_Timer = new HardwareTimer(TIM2);

//descobre canal e Timer associado ao pino 
// usado para ser compatível com qualquer STM32 e é mais prático
uint32_t pwm_channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PWM_PIN), PinMap_PWM));
TIM_TypeDef *PWM_Instance = (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(PWM_PIN), PinMap_PWM);

//declara objeto de timer associado ao PWM
HardwareTimer *PWM_Timer = new HardwareTimer(PWM_Instance);



//variavel para armazaenar largura do pulso pwm
uint32_t pulse_us = 0;

//armazena temporariamente intervalo de tempo entre
//pulso do sensor Hall 
uin32_t tempDelta_t = 0;  

//define flags e variaveis auxiliares
volatile bool enData1 = false;
uint32_t startTime; 
uint32_t lastPulse;


typedef struct {

  uint32_t cel_carga_meas[NUM_CELS];
  uint32_t timestamp_ms[NUM_CELS]; 

} async_data;


typedef struct {

  uint16_t corrente;
  uint16_t tensao;
  uint32_t delta_t;
  
} sync_data;


//função para fazer extensão de sinal de 24 bits para 32 bits
int convert24bitstoLong(int value24bits);

//declara funções das interrupções

//levanta flag quando qualquer HX711 abaixar os pinos
//dataPin
void getDataRequest(void);

//interrompe na borda de subida dos sensor de efeito Hall
void onHallPulse(void);

//interrupção períodica de amostragem
void getPeriodicSamples(void); 

//atualiza valor do pulso de pwm
//pulso varia de 1 ms até 2 ms
void updatePWMPulse(void);

//configura objeto HardwareTimer para gerar PWM desejado
void configPMW(void);

//configura Timer2 para gerar interrupções períodicas
void configSyncInterrupt(void);


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