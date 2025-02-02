/*
 * neopixel_anim.c
 *
 * Created: 13/02/2018 09:20:58
 *  Author: Steven
 */ 

 #include <stdlib.h>
 #include "neopixel.h"
 #include "neopixel_anim.h"



 /*!
 * \brief Pixel/Star status bits
 */
typedef union {
	struct {
		unsigned char active:1;
		unsigned char ramp_up:1;
		unsigned char not_used:6;
	} status_bits;
	unsigned char status;
} state_t;

 /*!
  * \brief The neopixel_anim_start_t structure defines the life and behavior of a "star". 
  * The pixel information maps the actual pixel and its colour information. The stars 
  * holds the status bits for whether the star is active and if it is in ramp_up mode 
  * i.e. increasing in intensity.
  */
 typedef struct {
	/*! The pixel being referenced. This contains the actual LED reference and the colour information. */
	pixel_type pixel;
	state_t state;
 } neopixel_anim_star_t;


 neopixel_anim_star_t star_buffer[MAX_BUFFERS][MAX_STARS];

/*!
 * \brief	Creates an effect of a droplet getting heavy and then falling.
 */
void neo_anim_commet(void){

	neopixel_fill(buffer[0], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00011111);
	neopixel_show(buffer[0]);

	// Pick a channel, any channel
	int channel  = rand();
	channel %= 0b00011111;
	uint8_t channel_mask = channel+1;

	// Set the pixel 0 to White and let the droplet gather some weight
	neopixel_setchannel(channel_mask);
	neopixel_setPixel(buffer[0], 0, HI_HUE, HI_HUE, HI_HUE);
	neopixel_show(buffer[0]);
	delay_ms(1000);
	neopixel_shift(buffer[0], true);
	neopixel_setPixel(buffer[0], 0, MED_HUE, MED_HUE, MED_HUE);
	neopixel_show(buffer[0]);
	delay_ms(750);
	neopixel_shift(buffer[0], true);
	neopixel_setPixel(buffer[0], 0, MED_HUE, MED_HUE, MED_HUE);
	neopixel_show(buffer[0]);
	delay_ms(500);
	neopixel_shift(buffer[0], true);
	neopixel_setPixel(buffer[0], 0, BASE_HUE, BASE_HUE, BASE_HUE);
	neopixel_show(buffer[0]);
	delay_ms(125);
	neopixel_shift(buffer[0], true);

	// Now let the pixels _fall_
	for(int j=0; j < NEOPIXELS_SIZE; j++){
		neopixel_show(buffer[0]);
		neopixel_shift(buffer[0], true);
		delay_ms(15);
		if (j > NEOPIXELS_SIZE - 6 ) {
		 	neopixel_setPixel(buffer[0], 0, NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
		}
	}
}


  void neo_anim_clear(uint8_t buff) {
	  for ( uint8_t star_idx = 0; star_idx < MAX_STARS; star_idx++) {
		  star_buffer[buff][star_idx].state.status_bits.active = false;
		  star_buffer[buff][star_idx].pixel.red = 0x00;
		  star_buffer[buff][star_idx].pixel.green = 0x00;
		  star_buffer[buff][star_idx].pixel.blue = 0x00;
		  // neopixel_setPixel(buffer[buff], star_buffer[buff][star_idx].pixel.pix, 0x00, 0x00, 0x00);
	  }
  }

 bool neo_anim_any_active(uint8_t buff_idx) {
	for( uint8_t star_idx = 0; star_idx < MAX_STARS; star_idx++) {
		if ( star_buffer[buff_idx][star_idx].state.status_bits.active ) {
			return true;
		}
	}
	return false;
 }

 bool star_buffer_contians( uint8_t buff, uint8_t pix ){
	for( uint8_t star_idx = 0; star_idx < MAX_STARS; star_idx++) {
		if( star_buffer[buff][star_idx].state.status_bits.active && star_buffer[buff][star_idx].pixel.pix == pix ){
			return true;
		}
	}
	return false;
 }

 uint8_t get_next_pixel_from_star_buffer(uint8_t buff) {
	 uint8_t next_pix = rand() % NEOPIXELS_SIZE;
	 while ( star_buffer_contians( buff, next_pix )) {
		 next_pix = rand() % NEOPIXELS_SIZE;
	 }
	 return next_pix;
 }

 uint8_t rand_colour() {
	 uint8_t colour = 0x00;
	 while( colour == 0x00 ){
		 colour = rand() % NEO_HUE_ADJ;
	 }
	 return colour;
 }


 /*!
 * \brief	Lights up a random number of "stars" MAX_STARS and increases their
 * brightness until FF for the given colour is reached and then reduces the
 * intensity.
 */
 void neo_anim_stars(uint8_t strip[], uint8_t buff_idx) {
	uint8_t channel = 0b00000001 << buff_idx;
// 	uint8_t toss;
	for ( uint8_t star_idx = 0; star_idx < MAX_STARS; star_idx++) {
		if (star_buffer[buff_idx][star_idx].state.status_bits.active != true) {
			star_buffer[buff_idx][star_idx].pixel.pix = get_next_pixel_from_star_buffer(buff_idx);
			star_buffer[buff_idx][star_idx].state.status_bits.active = true;
			star_buffer[buff_idx][star_idx].state.status_bits.ramp_up = true;
			star_buffer[buff_idx][star_idx].pixel.red = rand() % NEO_HUE_ADJ;
			star_buffer[buff_idx][star_idx].pixel.green = rand() % NEO_HUE_ADJ;
			star_buffer[buff_idx][star_idx].pixel.blue = rand() % NEO_HUE_ADJ;
			// If they are all 0x00, then force at least one colour to a value.
			if ( star_buffer[buff_idx][star_idx].pixel.red == 0x00 && star_buffer[buff_idx][star_idx].pixel.green == 0x00 && star_buffer[buff_idx][star_idx].pixel.blue == 0x00) {
				star_buffer[buff_idx][star_idx].state.status_bits.active = false;

			}
		}
	}
	neopixel_setchannel(channel);
	neopixel_show(strip);
	bool limit_reached = false;
	for( uint8_t star_idx = 0; star_idx < MAX_STARS; star_idx++){
		if ( star_buffer[buff_idx][star_idx].state.status_bits.active ) {
			if( star_buffer[buff_idx][star_idx].state.status_bits.ramp_up ) {
				limit_reached = neopixel_incPixelHue_with_limit(strip, star_buffer[buff_idx][star_idx].pixel );
				star_buffer[buff_idx][star_idx].state.status_bits.ramp_up ^= limit_reached;
			} else {
				limit_reached = neopixel_decrPixelHue_with_limit(strip, star_buffer[buff_idx][star_idx].pixel );
				if ( limit_reached ){
					star_buffer[buff_idx][star_idx].state.status_bits.active = false;
					star_buffer[buff_idx][star_idx].pixel.red = 0x00;
					star_buffer[buff_idx][star_idx].pixel.green = 0x00;
					star_buffer[buff_idx][star_idx].pixel.blue = 0x00;
					neopixel_setPixel(strip, star_buffer[buff_idx][star_idx].pixel.pix, 0x00, 0x00, 0x00);
				}
			}
			neopixel_setchannel(channel);
			neopixel_show(strip);
		}
	}
 }

 
 void neo_pixel_rampdown(uint8_t strip[], uint8_t buff_idx) {
 	uint8_t channel = 0b00000001 << buff_idx;
	for ( uint8_t star_idx = 0; star_idx < MAX_STARS; star_idx++) {
		bool limit_reached = neopixel_decrPixelHue_with_limit(strip, star_buffer[buff_idx][star_idx].pixel );
		if ( limit_reached ){
			star_buffer[buff_idx][star_idx].state.status_bits.active = false;
			star_buffer[buff_idx][star_idx].pixel.red = 0x00;
			star_buffer[buff_idx][star_idx].pixel.green = 0x00;
			star_buffer[buff_idx][star_idx].pixel.blue = 0x00;
			neopixel_setPixel(strip, star_buffer[buff_idx][star_idx].pixel.pix, 0x00, 0x00, 0x00);
		}
		neopixel_setchannel(channel);
		neopixel_show(strip);
	}
 }

 
void barber_pole(bool direction){

	uint8_t c1[] = { 0x04, 0x00, 0x00 };
	uint8_t c2[] = { 0x00, 0x02, 0x00 };

	neopixel_anim_init_bicolor(buffer[0], c1, c2);
	neopixel_anim_init_bicolor(buffer[1], c1, c2);
	neopixel_anim_init_bicolor(buffer[2], c1, c2);
	neopixel_anim_init_bicolor(buffer[3], c1, c2);
	neopixel_anim_init_bicolor(buffer[4], c1, c2);

	for(uint8_t i=0; i<2; i++){
		neopixel_shift(buffer[1], direction);
		neopixel_shift(buffer[2], direction);
		neopixel_shift(buffer[3], direction);
		neopixel_shift(buffer[4], direction);
	}

	for(uint8_t i=0; i<2; i++){
		neopixel_shift(buffer[2], direction);
		neopixel_shift(buffer[3], direction);
		neopixel_shift(buffer[4], direction);
	}

	for(uint8_t i=0; i<2; i++){
		neopixel_shift(buffer[3], direction);
		neopixel_shift(buffer[4], direction);
	}

	for(uint8_t i=0; i<2; i++){
		neopixel_shift(buffer[4], direction);
	}

	for (uint8_t i = 0; i < 240; i++) {

		neopixel_setchannel((uint8_t)0b00000001);
		neopixel_show(buffer[0]);

		neopixel_setchannel((uint8_t)0b00000010);
		neopixel_show(buffer[1]);

		neopixel_setchannel((uint8_t)0b00000100);
		neopixel_show(buffer[2]);

		neopixel_setchannel((uint8_t)0b00001000);
		neopixel_show(buffer[3]);

		neopixel_setchannel((uint8_t)0b00010000);
		neopixel_show(buffer[4]);

		neopixel_shift(buffer[0], direction);
		neopixel_shift(buffer[1], direction);
		neopixel_shift(buffer[2], direction);
		neopixel_shift(buffer[3], direction);
		neopixel_shift(buffer[4], direction);

		delay_ms(50);
	}
}


void chevron(bool direction){

	uint8_t c1[] = { 0x04, 0x04, 0x04 };
	uint8_t c2[] = { 0x03, 0x00, 0x00 };

	neopixel_anim_init_bicolor(buffer[0], c1, c2);
	neopixel_anim_init_bicolor(buffer[1], c1, c2);
	neopixel_anim_init_bicolor(buffer[2], c1, c2);

	for(uint8_t i=0; i<2; i++){
		neopixel_shift(buffer[1], direction);
		neopixel_shift(buffer[2], direction);
	}

	for(uint8_t i=0; i<2; i++){
		neopixel_shift(buffer[2], direction);
	}

	for (uint8_t i = 0; i < 240; i++) {

		neopixel_setchannel((uint8_t)0b00010001);
		neopixel_show(buffer[0]);

		neopixel_setchannel((uint8_t)0b00001010);
		neopixel_show(buffer[1]);

		neopixel_setchannel((uint8_t)0b00000100);
		neopixel_show(buffer[2]);

		neopixel_shift(buffer[0], direction);
		neopixel_shift(buffer[1], direction);
		neopixel_shift(buffer[2], direction);

		delay_ms(50);
	}
}




void colour_shuffle(void){

	neopixel_fill(buffer[0], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_fill(buffer[2], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);

	for (int i = 0; i < 15; i++){
		neopixel_setchannel((uint8_t)0b00010101);
		neopixel_show(buffer[0]);
		neopixel_setchannel((uint8_t)0b00001010);
		neopixel_show(buffer[1]);

		delay_ms(500);

		neopixel_setchannel((uint8_t)0b00010101);
		neopixel_show(buffer[1]);
		neopixel_setchannel((uint8_t)0b00001010);
		neopixel_show(buffer[2]);

		delay_ms(500);
		neopixel_setchannel((uint8_t)0b00010101);
		neopixel_show(buffer[2]);
		neopixel_setchannel((uint8_t)0b00001010);
		neopixel_show(buffer[0]);

		delay_ms(500);


	}
}





void worm(void){

	neopixel_fill(buffer[0], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00011111);
	neopixel_show(buffer[0]);

	neopixel_setchannel((uint8_t)0b00000001);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000010);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00001000);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00010000);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);

	
	neopixel_setchannel((uint8_t)0b00000001);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000010);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00001000);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00010000);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);


	neopixel_setchannel((uint8_t)0b00000001);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000010);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00001000);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00010000);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, true, 10);



	neopixel_setchannel((uint8_t)0b00000001);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);
	neopixel_setchannel((uint8_t)0b00000010);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);
	neopixel_setchannel((uint8_t)0b00001000);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);
	neopixel_setchannel((uint8_t)0b00010000);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);


	delay_ms(1000);
}


void worm2(void){

	neopixel_fill(buffer[0], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00011111);
	neopixel_show(buffer[0]);


	neopixel_setchannel((uint8_t)0b00010001);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00001010);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF, true, 10);



	neopixel_setchannel((uint8_t)0b00010001);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00001010);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], BASE_HUE, NEO_ALL_OFF, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);


	neopixel_setchannel((uint8_t)0b00010001);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00001010);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE, true, 10);



	neopixel_setchannel((uint8_t)0b00010001);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);
	neopixel_setchannel((uint8_t)0b00001010);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, false, 10);

	neopixel_fill(buffer[1], NEO_ALL_OFF, NEO_ALL_OFF, BASE_HUE);
	neopixel_setchannel((uint8_t)0b00000100);
	neopixel_anim_wipe(buffer[1], NEO_ALL_OFF, BASE_HUE, NEO_ALL_OFF, true, 10);
	
	delay_ms(1000);
}

/*!
 * \brief	Rolls the pixels in a direction, the number of positions.
 * 
 * rolls the pixels one direction or the other the number of positions
 * with an intermediate animation of delay ms. The neopixel_show 
 * function will be called.

*/
void neopixel_anim_pixel_chaser(uint8_t strip[], bool direction, int positions, int delay){
	for( uint8_t i = 0; i < positions; i++)
	{
		neopixel_shift(strip, direction);
		neopixel_show(strip);
		delay_ms(delay);
	}
}


/*!
 * \brief	Initialise the pixels with a sequence of two colours.
 *
 *	The colours are divided in groups of 10 pixels.
 */
void neopixel_anim_init_bicolor(uint8_t strip[], uint8_t colourA[], uint8_t colourB[]){

	uint8_t red_color = 0, green_color = 0, blue_color = 0;

	for (int i = 0; i < NEOPIXELS_SIZE; i++)
	{
		if ( i <= 9) {
			red_color = colourA[0];
			green_color = colourA[1];
			blue_color = colourA[2];
		}
		if ( i >= 10 && i <= 19) {
			red_color = colourB[0];
			green_color = colourB[1];
			blue_color = colourB[2];
		}
		if ( i >= 20 && i <= 29) {
			red_color = colourA[0];
			green_color = colourA[1];
			blue_color = colourA[2];
		}
		if ( i >= 30 && i <= 39) {
			red_color = colourB[0];
			green_color = colourB[1];
			blue_color = colourB[2];
		}
		if ( i >= 40 && i <= 49) {
			red_color = colourA[0];
			green_color = colourA[1];
			blue_color = colourA[2];
		}
		if ( i >= 50 ) {
			red_color = colourB[0];
			green_color = colourB[1];
			blue_color = colourB[2];
		}
		neopixel_setPixel(strip, i, red_color, green_color, blue_color);
	}
}




/*!
 *	\brief	Intialise the buffer with a "pulsed rainbow" type 
 *			sequence.
 *
 *	The "rainbow" is dived over the 60 pixels. A group of 10 
 *	pixels for each colour group. The "pulse"effect is to have
 *	the middle pixel at the highest hue intensity.
 *
 */
void neopixel_anim_init_rainbow_pulse(uint8_t strip[], uint8_t hue) {
	uint8_t red_color = 0;
	uint8_t green_color = 0;
	uint8_t blue_color = 0;

	int8_t grade = -5;
	uint8_t colour = 100;
	uint8_t page;
	uint8_t grade_factor = hue / 5 ;

	for(int group = 0; group < 6; group++) {
		grade = -5;
		page = group * 10;
		for(int pix = 0; pix < 5; pix++) {
			colour = (hue + (grade * grade_factor));
			grade++;
			switch( group ) {
				case 0:
					red_color = colour;
					green_color = NEO_ALL_OFF;
					blue_color = colour;
					break;
				case 1:
					red_color = colour;
					green_color = NEO_ALL_OFF;
					blue_color = NEO_ALL_OFF;
					break;
				case 2:
					red_color = colour;
					green_color = colour;
					blue_color = NEO_ALL_OFF;
					break;
				case 3:
					red_color = NEO_ALL_OFF;
					green_color = colour;
					blue_color = NEO_ALL_OFF;
					break;
				case 4:
					red_color = NEO_ALL_OFF;
					green_color = colour;
					blue_color = colour;
					break;
				case 5:
					red_color = NEO_ALL_OFF;
					green_color = NEO_ALL_OFF;
					blue_color = colour;
					break;
				default:
					break;
			}
			neopixel_setPixel(strip, page+pix, red_color, green_color, blue_color);
		}
		grade = 0;
		for(uint8_t pix = 5; pix < 11; pix++){
			colour = hue + (grade * grade_factor);
			grade--;
			switch( group ){
				case 0:
					red_color = colour;
					green_color = NEO_ALL_OFF;
					blue_color = colour;
					break;
				case 1:
					red_color = colour;
					green_color = NEO_ALL_OFF;
					blue_color = NEO_ALL_OFF;
					break;
				case 2:
					red_color = colour;
					green_color = colour;
					blue_color = NEO_ALL_OFF;
					break;
				case 3:
					red_color = NEO_ALL_OFF;
					green_color = colour;
					blue_color = NEO_ALL_OFF;
					break;
				case 4:
					red_color = NEO_ALL_OFF;
					green_color = colour;
					blue_color = colour;
					break;
				case 5:
					red_color = NEO_ALL_OFF;
					green_color = NEO_ALL_OFF;
					blue_color = colour;
					break;
				default:
					break;
			}
			neopixel_setPixel(strip, page+pix, red_color, green_color, blue_color);
		}
	}
}

/*!
 * \brief	Initialise the pixels with a "rainbow" series.
 *
 *	The colours are divided over the 60 pixels with six colours allocated
 *	to each group of 10 pixels.
 */
void neopixel_anim_init_rainbow(uint8_t strip[], uint8_t hue){
	uint8_t red_color = 0;
	uint8_t green_color = 0;
	uint8_t blue_color = 0;

	for (int i = 0; i < NEOPIXELS_SIZE; i++)
	{
		if ( i <= 9) {
			red_color = hue;
			green_color = NEO_ALL_OFF;
			blue_color = hue;
		}
		if ( i >= 10 && i <= 19) {
			red_color = hue;
			green_color = NEO_ALL_OFF;
			blue_color = NEO_ALL_OFF;
		}
		if ( i >= 20 && i <= 29) {
			red_color = hue;
			green_color = hue;
			blue_color = NEO_ALL_OFF;
		}
		if ( i >= 30 && i <= 39) {
			red_color = NEO_ALL_OFF;
			green_color = hue;
			blue_color = NEO_ALL_OFF;
		}
		if ( i >= 40 && i <= 49) {
			red_color = NEO_ALL_OFF;
			green_color = hue;
			blue_color = hue;
		}
		if ( i >= 50 ) {
			red_color = NEO_ALL_OFF;
			green_color = NEO_ALL_OFF;
			blue_color = hue;
		}
		neopixel_setPixel(strip, i, red_color, green_color, blue_color);
	}
}

/*!
 * \brief	Initialise the pixels with a "rainbow" series.
 *
 *	The colours are divided over the 60 pixels with six colours allocated
 *	to each group of 10 pixels.
 */
void neopixel_anim_init_random(uint8_t strip[], uint8_t hue){
	uint8_t red_color = 0;
	uint8_t green_color = 0;
	uint8_t blue_color = 0;

	uint8_t colour_bias = 0; 

	for (uint8_t i = 0; i < NEOPIXELS_SIZE; i++)
	{

	   red_color = rand() % hue;
	   blue_color = rand() % hue;
	   green_color = rand() % hue;

	   colour_bias = rand() % 3;
	   switch(colour_bias){
	   case 1:
			red_color = 0;
			break;
	   case 2:
			green_color = 0;
			break;
	   case 3:
			blue_color = 0;
			break;
	   default:
			red_color = 0;
			green_color = 0;
			blue_color = 0;
			break;
	   }
	   neopixel_setPixel(strip, i, red_color, green_color, blue_color);
	}
}


/*!
 *	\brief	Fills the pixel strip with a colour
 *
 *	The colour is filled based on the direction and with an animation delay.
 *	The neopixel_show will be called.
 */
void neopixel_anim_wipe(uint8_t strip[], uint8_t red, uint8_t green, uint8_t blue, bool direction, int delay){

	uint8_t pixel = ( direction )? 0: NEOPIXELS_SIZE-1;
	for(uint8_t i = 0; i < NEOPIXELS_SIZE; i++){
		neopixel_setPixel(strip, pixel , red, green, blue);
		neopixel_show(strip);
		neopixel_shift(strip, direction);
		delay_ms(delay);
	}
}

/*!
 * \brief The wipe function that will wipe in a "rainbow"
 *
 */
void neopixel_anim_rainbow_wipe(uint8_t strip[], uint8_t hue, bool direction, int delay){

	uint8_t pixel = ( direction )? NEOPIXELS_SIZE - 1: 0;
	uint8_t red, green, blue;
	red = 0;
	green = 0;
	blue = 0;
	for(uint8_t i = 0; i < NEOPIXELS_SIZE; i++){
		if( direction ) {
			if ( i <= 9) {
				red = hue;
				green = NEO_ALL_OFF;
				blue = hue;
			}
			if ( i >= 10 && i <= 19) {
				red = hue;
				green = NEO_ALL_OFF;
				blue = NEO_ALL_OFF;
			}
			if ( i >= 20 && i <= 29) {
				red = hue;
				green = hue;
				blue = NEO_ALL_OFF;
			}
			if ( i >= 30 && i <= 39) {
				red = NEO_ALL_OFF;
				green = hue;
				blue = NEO_ALL_OFF;
			}
			if ( i >= 40 && i <= 49) {
				red = NEO_ALL_OFF;
				green = hue;
				blue = hue;
			}
			if ( i >= 50 ) {
				red = NEO_ALL_OFF;
				green = NEO_ALL_OFF;
				blue = hue;
			}

			neopixel_setPixel(strip, pixel, red, green, blue);
			neopixel_show(strip);
			neopixel_shift(strip, direction);
		} else {
			if ( i <= 9) {
				red = hue;
				green = NEO_ALL_OFF;
				blue = hue;
			}
			if ( i >= 10 && i <= 19) {
				red = hue;
				green = NEO_ALL_OFF;
				blue = NEO_ALL_OFF;
			}
			if ( i >= 20 && i <= 29) {
				red = hue;
				green = hue;
				blue = NEO_ALL_OFF;
			}
			if ( i >= 30 && i <= 39) {
				red = NEO_ALL_OFF;
				green = hue;
				blue = NEO_ALL_OFF;
			}
			if ( i >= 40 && i <= 49) {
				red = NEO_ALL_OFF;
				green = hue;
				blue = hue;
			}
			if ( i >= 50 ) {
				red = NEO_ALL_OFF;
				green = NEO_ALL_OFF;
				blue = hue;
			}

			neopixel_setPixel(strip, 0, red, green, blue);
			neopixel_setPixel(strip, NEOPIXELS_SIZE - 1, red, green, blue);
			neopixel_show(strip);
			neopixel_shift(strip, direction);
		}
		delay_ms(delay);

	}
}

