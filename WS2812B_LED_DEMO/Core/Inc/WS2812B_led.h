/*
 * WS2812B_led.h
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 *      Header file for the WS2812B LED strip module
 */

#ifndef INC_WS2812B_LED_H_
#define INC_WS2812B_LED_H_
#include "main.h"
#include <string.h>
extern SPI_HandleTypeDef hspi1;
//#define GREEN 0xDB6DB6
#define GREEN (0xFF)<<16
#define RED (0xFF)<<8
#define BLUE (0xFF)
#define RESET 0



typedef struct
{
	uint8_t led_number; //leds total count
	uint32_t color; //leds color
}led_struct_handle_t;


void WS2812B_send(led_struct_handle_t* led_handle);

#endif /* INC_WS2812B_LED_H_ */
