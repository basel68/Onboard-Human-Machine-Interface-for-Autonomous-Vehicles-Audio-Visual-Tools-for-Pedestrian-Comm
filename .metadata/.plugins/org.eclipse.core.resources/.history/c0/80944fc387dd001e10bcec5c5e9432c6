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
//maximum number of leds
#define MAX_LED 20
//led off macro
#define OFF 0
//led color macros
#define GREEN 0xFF0000
#define RED 0x00FF00
#define BLUE 0x0000FF
extern uint8_t datasentflag;
extern TIM_HandleTypeDef htim1;

//led strip control functions
void Set_LED (uint8_t LEDnum, uint8_t Red, uint8_t Green, uint8_t Blue);
void WS2812_Send (uint32_t color);
#endif /* INC_WS2812B_LED_H_ */
