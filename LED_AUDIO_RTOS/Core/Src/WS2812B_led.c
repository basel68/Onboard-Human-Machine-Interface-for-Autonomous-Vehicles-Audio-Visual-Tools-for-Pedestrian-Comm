/*
 * WS2812B_led.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */


#include "WS2812B_led.h"
uint16_t pwmData[(24*MAX_LED)+50];
uint8_t LED_Data[MAX_LED][4];

/**
  * @brief  set the RGB colors of the led number specified
  * @param  led number
  * @param  red degree
  * @param  green degree
  * @param blue degree
  */
void Set_LED (uint8_t LEDnum, uint8_t Red, uint8_t Green, uint8_t Blue){
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Green;
	LED_Data[LEDnum][2] = Red;
	LED_Data[LEDnum][3] = Blue;
}

/**
  * @brief  set the correct values for the red green blue colors according to the ws2812b datasheet and then sends daat through pwm pin
  */
void WS2812_Send (void)
{
	uint32_t indx=0;
	uint32_t color;


	for (int i= 0; i<MAX_LED; i++)
	{

		color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));


		for (int i=23; i>=0; i--)
		{
			if (color&(1<<i))
			{
				pwmData[indx] = 60;  // 2/3 of 90
			}

			else pwmData[indx] = 30;  // 1/3 of 90

			indx++;
		}

	}
// 50us of 0s delay as required in datasheet
	for (int i=0; i<48; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}
//start transmission to led strip
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	while (!datasentflag){};
	datasentflag = 0;
}
