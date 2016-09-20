/* header file for the newly developed library for decoding PPM-Signals	*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define CHANNELAMOUNT 6

volatile uint16_t channelstorage[CHANNELAMOUNT];
volatile uint8_t channel_digitized_indicator;
volatile uint8_t pinastateold;
volatile uint8_t pinbstateold;
volatile uint8_t changedpin;
volatile uint16_t now;
volatile uint16_t ppm_duration;
volatile uint16_t ppm_starttime[CHANNELAMOUNT];
volatile uint8_t timer_1_overflow_indicator;
volatile uint8_t timer_1_overflow_indicator_buffer[CHANNELAMOUNT];

uint8_t return_channel_value(uint8_t channel,uint8_t data[],uint8_t start);
void init_ppm_digitize(void);

