//** G L O B A L  D E F I N I T I O N S ***************************************/
#define ADCreadlatch() ((void) (/* SPICLK */ LATCbits.LATC3 = 0))
//#define ReadSignals() (ADCreadall(adcresult))

//** G L O B A L  V A R I A B L E S *******************************************/
extern uint16_t adcresult[8]; // Justification that the variable is Global

//** G L O B A L  P R O T O T Y P E S *****************************************/
void ADCreadprepare(uint8_t channel);
uint16_t ADCreadextract(void);
//uint16_t ADCread(uint8_t channel);
//void ADCreadall(uint16_t all[8]);

void ReadExternalADC( void );
//*****************************************************************************/
