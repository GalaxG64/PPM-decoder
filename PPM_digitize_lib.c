/* newly designed library for decoding PPM Signals into discrete values */
/* uses Arrays to circumvent 2-Byte-Variables				*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "PPM_digitize_lib.h"

void init_ppm_digitize(void)
{
	//enables all the accesable pincahange-interrupts on the Chip
	GIMSK|=(1<<PCIE0);//|(1<<PCIE1);
	//selects the pins that shal contribute to the PCINT interrupts
	PCMSK1|=(1<<PCINT9)|(1<<PCINT10);
	PCMSK0|=/*(1<<PCINT7)|(1<<PCINT2)|*/(1<<PCINT1)|(1<<PCINT0);
	//enable timer interrupts
	TIMSK1|=(1<<TOIE1);
	//starts the timer for the calculations of the PPM signal length
	TCCR1B|=(1<<CS10);
	//initialize variables 
	pinastateold=PINA;
	pinbstateold=PINB;
	//set the DDR according to the Channels used
	DDRA&=~((1<<PA0)|(1<<PA1));
	DDRB&=~((1<<PB1)|(1<<PB2));
}

uint8_t return_channel_value(uint8_t channel,uint8_t data[],uint8_t start)
{
	if(channel_digitized_indicator&(1<<channel)){
		data[start+1]= (uint8_t) (channelstorage[channel]>>8);
		data[start]= (uint8_t) (channelstorage[channel]);
		channel_digitized_indicator&=~(1<<channel);
		return 1;
	}
	else{
		return 0;
	}
}

ISR(PCINT0_vect)
{
	now=TCNT1;
	changedpin=PINA^pinastateold;
	pinastateold=PINA;
	uint8_t i=0;
	while(!((changedpin>>i)&0x01)){
		i++;
	}
	if(PINA&(1<<i)){
		if(i==7){
			i=5;
		}
		ppm_starttime[i]=now;
		timer_1_overflow_indicator_buffer[i]=timer_1_overflow_indicator;
	}
	else{
		if(i==7){
			i=5;
		}
		if(timer_1_overflow_indicator_buffer[i]==timer_1_overflow_indicator){
			ppm_duration=now-ppm_starttime[i];
			channelstorage[i]= ppm_duration;
		}
		else{
			ppm_duration=now+(0xFFFF-ppm_starttime[i]);
			channelstorage[i]= ppm_duration;
		}
		channel_digitized_indicator|=(1<<i);
	}
}

ISR(PCINT1_vect)
{
	now=TCNT1;
	changedpin=PINB^pinbstateold;
	pinbstateold=PINB;
	uint8_t i=0;
	while((changedpin>>i)&0x01){
		i++;
	}
	if(PINB&(1<<i)){
		i+=2;
		ppm_starttime[i]=now;
		timer_1_overflow_indicator_buffer[i]=timer_1_overflow_indicator;
	}
	else{
		i+=2;
		if(timer_1_overflow_indicator_buffer[i]==timer_1_overflow_indicator){
			ppm_duration=now-ppm_starttime[i];
			channelstorage[i]= ppm_duration;
		}
		else{
			ppm_duration=now+(0xFFFF-ppm_starttime[i]);
			channelstorage[i]= ppm_duration;
		}
	}	
}

ISR(TIM1_OVF_vect)
{
	timer_1_overflow_indicator=1-timer_1_overflow_indicator;
}
