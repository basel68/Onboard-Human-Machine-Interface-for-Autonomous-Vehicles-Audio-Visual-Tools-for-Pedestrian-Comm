/*
 * Task_handler.c
 *
 *  Created on: Mar 24, 2024
 *      Author: BASSEL
 */
#include "main.h"
uint16_t* PASS_audio_data_16bit = ( uint16_t*)(&PASS_audio_wav);
uint16_t* STOP_audio_data_16bit = ( uint16_t*)(&STOP_audio_wav);
void led_strip_handler(){
	while(1){//alternates between red green blue


		  for(int i=0;i<20;i++){
			   Set_LED(i, 0, 0, 0);
			  }
		   WS2812_Send();
		   HAL_Delay(1000);
		   for(int i=0;i<20;i++){
			   Set_LED(i, 0,0, 255);
			  }
		  WS2812_Send();
		  HAL_Delay(1000);
		  for(int i=0;i<20;i++){
			   Set_LED(i, 0, 0, 0);
			  }

		   WS2812_Send();
		   for(int i=0;i<20;i++){
			   Set_LED(i, 255, 0, 0);
			  }
		  WS2812_Send();
		  HAL_Delay(1000);
		  for(int i=0;i<20;i++){
			   Set_LED(i, 0, 0, 0);
			  }
		  UBaseType_t uxHighWaterMark;
		  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		  printf("LedTask unused stack: %lu words\n", uxHighWaterMark);
		   WS2812_Send();

		   for(int i=0;i<20;i++){
			   Set_LED(i, 0, 255,0);
			  }
			  WS2812_Send();
			  HAL_Delay(1000);
		}

}
void Audio_handler(){
	while(1){
	    // Play the audio

		 Audio_play(PASS_audio_data_16bit, PASS_audio_wav_size);
		  UBaseType_t uxHighWaterMark;
		  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		  printf("AudioTask1 unused stack: %lu words\n", uxHighWaterMark);
		 HAL_Delay(1500);

		 Audio_play(STOP_audio_data_16bit, STOP_audio_wav_size);

		  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		  printf("AudioTask2 unused stack: %lu words\n", uxHighWaterMark);
		 HAL_Delay(1500);
	}
}
