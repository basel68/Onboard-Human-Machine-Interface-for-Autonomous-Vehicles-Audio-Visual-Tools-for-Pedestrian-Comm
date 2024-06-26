/*
 * WS2812B_led.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */


#include "WS2812B_led.h"
static void WS2812B_spi(uint32_t color);

static void WS2812B_spi(uint32_t color){
	uint8_t data[24];
	memset(data,0, sizeof(data));
//	data[0]=color>>16;
//	data[1]=color>>8;
//	data[2]=color;
//	for(int i=9;i>0;i--){
//		HAL_SPI_Transmit(&hspi1, data, sizeof(data), 1000);
//	}
	int index=0;
	for(int i=23;i>=0;i--){
		if((color>>i)& SET){
			data[index++]=0b110;
		}
		else{
			data[index++]=0b100;
		}
	}
		HAL_SPI_Transmit(&hspi1, data, sizeof(data), 1000);

}
void WS2812B_send(led_struct_handle_t* led_handle){
	uint8_t leds_number=led_handle->led_number;
	uint32_t color=led_handle->color;
	while(leds_number>0){
		WS2812B_spi(color);
		leds_number--;
	}
	HAL_Delay(1);
}

