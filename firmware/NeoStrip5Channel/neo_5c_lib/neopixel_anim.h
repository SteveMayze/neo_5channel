/*
 * neopixel_anim.h
 *
 * Created: 13/02/2018 09:20:39
 *  Author: Steven
 */ 


#ifndef NEOPIXEL_ANIM_H_
#define NEOPIXEL_ANIM_H_

#include <stdbool.h>
#include <neopixel.h>

#define BASE_HUE ((uint8_t) 0x03)
#define MED_HUE  ((uint8_t) 0x06)
#define HI_HUE   ((uint8_t) 0x0C)
#define MAX_BUFFERS 5
#define MAX_STARS (NEOPIXELS_SIZE / 3)
#define NEO_ANIM_MIN_CYCLES 500
#define NEO_ANIM_MAX_CYCLES 2000
#define NEO_ANIM_MAX_GRADIENT 100
#define COMMET_SIZE (NEOPIXELS_SIZE / 10)


uint8_t buffer[MAX_BUFFERS][neopixel_buffer_size];


void neopixel_anim_init_bicolor(uint8_t strip[], uint8_t colourA[], uint8_t colourB[]);

void neopixel_anim_wipe(uint8_t strip[], uint8_t red, uint8_t green, uint8_t blue, bool direction, int delay);

void neopixel_anim_rainbow_wipe(uint8_t strip[], uint8_t hue, bool direction, int delay);

void neopixel_anim_init_rainbow_pulse(uint8_t strip[], uint8_t hue);

void neopixel_anim_init_rainbow(uint8_t strip[], uint8_t hue);

void neopixel_anim_pixel_chaser(uint8_t strip[], bool direction, int positions, int delay);

 void neo_anim_clear(uint8_t buff);

void neo_anim_comet(void);

bool neo_anim_stars(uint8_t buff_idx, bool finish_up);


#endif /* NEOPIXEL_ANIM_H_ */