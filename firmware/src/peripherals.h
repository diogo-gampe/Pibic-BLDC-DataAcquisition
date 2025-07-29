
#ifndef PERIPHERALS_H
#define PERIPHERALS_H

/*  ~ Definindo pinos, funções de configurações e interrupções gerados pelos periféricos
    ~ ADC + DMA + Timer3 + Timer2 + Pino do Efeito Hall

*/

//*Define pinos
//!!NÃO MEXER NO PINO PWM!!! PODE DESCONFIGURAR TIMER DA INTERRUPÇÃO PERIÓDICA
#define PWM_PIN PB1
#define ADC_CURRENT_PIN PA0
#define ADC_VOLT_PIN PA1
#define HALL_PIN PB8

//variáveis para configurar PWM 
#define PWM_FREQUENCY 1280 // hz
#define PWM_PERIOD (1000000/PWM_FREQUENCY) //Periodo em microsegundos
#define MAX_PULSE_US PWM_PERIOD //us
#define MIN_PULSE_US 0 //us

//T -> período de amostragem para ISR de amostragem
#define T 1024 //us

// ----------- Definindo variáveis dos periféricos que serão usadas na main (integrationTest.ino)-------------

//Objetos Hardware Timer do stm32duino um para ISR e outro para PWM
extern HardwareTimer *Update_Timer; 
extern HardwareTimer *PWM_Timer;

extern uint32_t elapsedTest_Sync;  
extern volatile uint16_t adcBuffer[2];
extern volatile uint32_t isrSyncCounter;
extern volatile bool syncDataReady; 

//define tipo de struct para armazenar valores das medições síncronas
typedef struct {

  uint16_t corrente;
  uint16_t tensao;
  uint32_t delta_t;
  bool erro; 
  
} sync_data;

//variável externa para armazenar dados de ISR
extern volatile sync_data ISR_data;  

void onHallPulse(void);

//funções para habilitar e desabilitar TODAS interrupções
void Interrupts(void);
void disableInterrupts(void);

//função de interrupção períodica de amostragem
void getPeriodicSamples(void); 

//atualiza pulso -> varia de 1 ms até 2 ms (ESC de Standard PWM)
void updatePWMPulse(void);

//configura Timer2 para gerar interrupções períodicas
void configSyncInterrupt(void);

//configura objeto HardwareTimer para gerar PWM desejado
void configPWM(void);

//configura a nível de registrador funcionamento ADC
void configADC(void);

//configura DMA para armazenar valores ADC.
void configDMA(void);
#endif