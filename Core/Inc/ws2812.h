/*
 * ws2812.h
 *
 *  Created on: Oct 26, 2024
 *      Author: Pawel
 */

#ifndef INC_WS2812_H_
#define INC_WS2812_H_

#include "main.h"


#define zero 255
#define one 240

#define WS2811_LEDS 301

typedef struct ws2811_color {
	uint8_t red, green, blue;
	}ws2812b_color;



void ws2811_refresh(void);
void ws2811_refresh_IRQ(void);
void ws2811_set_diode_HSV(int16_t diode_id, uint16_t hue, uint8_t saturation, uint8_t brightness);		// numer diody, kolor, nasycenie, jasność
void ws2811_color(void);
void WS2811_rainbow(uint16_t first_led, uint16_t last_led);
void WS2811_rainbow_puls( uint16_t first_led, uint16_t last_led);
void WS2811_set_color_HSV(uint16_t color, uint8_t saturation, uint8_t brightness, uint16_t first_led, uint16_t last_led);
void WS2811_flash(uint16_t color, uint8_t brightness, uint16_t first_led, uint16_t last_led);
void WS2811_flash_color(uint16_t first_led, uint16_t last_led, uint32_t time);
void Refresch_SysTick(uint16_t time);
void WS2811_snake(void);
void WS2811_rainbow_flow(void);
void color_change (uint16_t time);
#endif /* INC_WS2812_H_ */
