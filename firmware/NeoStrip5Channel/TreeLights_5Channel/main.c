/*
 * NeoStrip5Channel_plus.c
 *
 * Created: 13/02/2018 09:46:43
 * Author : Steven
 */ 

#include "stdlib.h"
#include "time.h"
#include <avr/io.h>

#include <neopixel_anim.h>

/*! The setting for the CLKCTRL.MCLKCTRLB register */
#define _MAIN_CLOCK 0x00


// #define _NEO_RAIN_MAIN

#define _NEOPIXEL_ANIM_CHANNEL_MAIN



#ifdef _NEOPIXEL_ANIM_CHANNEL_MAIN
/************************************************************************/
/* The main control loop                                                */
/************************************************************************/
int main(void)
{

	// This register is protected and can not be changed until the CPP register in the CPU
	// is written with the signature of 0xD8 ... I think I need to insert some assembly code here
	// This PEN flag is reset, this means that the Source clock is fed right through and not pre-scaled.
	// This needs to be checked for the electrical characteristics is such that the full 5V is required.
	// uint8_t strips[NEOPIXELS_SIZE *3];

	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = _MAIN_CLOCK;

	neopixel_init();
	srand(time(NULL));
	uint16_t cycle = 0;
	uint16_t cycle_limit = rand() % 1000;
	cycle_limit = (cycle_limit < 100? 100: cycle_limit);
	while(true){

		if ( cycle > cycle_limit ) {
			cycle = 0;
			cycle_limit = rand() % 1000;
			cycle_limit = ( cycle_limit < 100? 100: cycle_limit);

			delay_ms(1000);
			neo_anim_commet();
			delay_ms(1000);
		}

		for ( int buff_idx = 0; buff_idx < MAX_BUFFERS;  buff_idx++) {
			neo_anim_stars(buffer[buff_idx], buff_idx);
		}
		//delay_ms(1000);
		uint8_t gradient = rand() % NEO_ANIM_MAX_GRADIENT;
		gradient = gradient < 2? 2: gradient;
		delay_ms(gradient);
		cycle++;

	}
	return 0;
}

#endif

#ifdef _NEO_RAIN_MAIN
int main(void)
{
	while( true ) {
		neo_anim_commet();
		delay_ms(3000);
	}
	return 0;
}
#endif
