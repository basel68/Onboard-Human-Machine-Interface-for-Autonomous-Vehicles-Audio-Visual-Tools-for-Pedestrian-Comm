/*
 * Task_handler.c
 *
 *  Created on: Mar 24, 2024
 *      Author: BASSEL
 */
#include "main.h"
uint16_t* PASS_audio_data_16bit = ( uint16_t*)(&PASS_audio_wav);
uint16_t* STOP_audio_data_16bit = ( uint16_t*)(&STOP_audio_wav);
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
void led_strip_audio_handler(){
	  start_time_LED=__HAL_TIM_GET_COUNTER(&htim2);
	  start_time_LED_big=__HAL_TIM_GET_COUNTER(&htim3);
		   for(int i=0;i<5;i++){
			   Set_LED(i, 0,0, 255);
			  }
		   for(int i=5;i<20;i++){
			   Set_LED(i, 255,0, 0);
			  }
		  WS2812_Send(20);
//		  HAL_Delay(1000);
		  start_time_AUDIO_big=__HAL_TIM_GET_COUNTER(&htim3);
		  Audio_handler();
		  for(int i=0;i<5;i++){
			   Set_LED(i, 0, 0, 0);
			  }
		  for(int i=5;i<20;i++){
			   Set_LED(i, 255,0, 0);
			  }

		   WS2812_Send(5);
		   HAL_Delay(1000);

		   for(int i=0;i<5;i++){
			   Set_LED(i, 0, 0, 255);
			  }
		  WS2812_Send(5);
//		  HAL_Delay(1000);
		  Audio_handler();
		  for(int i=0;i<20;i++){
			   Set_LED(i, 0, 0, 0);
			  }
		   WS2812_Send(20);
		   end_time_LOOP=__HAL_TIM_GET_COUNTER(&htim3);



}
void Audio_handler(){

	    // Play the audio

		 Audio_play(STOP_audio_data_16bit, STOP_audio_wav_size);

		 HAL_Delay(1500);

}
