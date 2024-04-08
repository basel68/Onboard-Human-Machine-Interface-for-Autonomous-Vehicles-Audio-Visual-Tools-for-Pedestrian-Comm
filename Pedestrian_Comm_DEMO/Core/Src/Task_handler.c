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
	uint32_t pulNotificationValueled;
	while(1){//displays red visual light

		xTaskNotifyWait(0,0,&pulNotificationValueled,portMAX_DELAY);

		   for(int i=0;i<5;i++){
			   Set_LED(i, 0,0, 255);
			  }
		   for(int i=5;i<20;i++){
			   Set_LED(i, 255,0, 0);
			  }
		  WS2812_Send();
		  vTaskDelay(1000);
		  for(int i=0;i<5;i++){
			   Set_LED(i, 0, 0, 0);
			  }
//		  for(int i=5;i<20;i++){
//			   Set_LED(i, 255,0, 0);
//			  }

		   WS2812_Send();
		   vTaskDelay(1000);
		   for(int i=0;i<5;i++){
			   Set_LED(i, 0, 0, 255);
			  }
//		   for(int i=5;i<20;i++){
//			   Set_LED(i, 255,0, 0);
//			  }
		  WS2812_Send();
		  vTaskDelay(1000);
		  for(int i=0;i<20;i++){
			   Set_LED(i, 0, 0, 0);
			  }
//		  UBaseType_t uxHighWaterMark;
//		  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//		  printf("LedTask unused stack: %lu words\n", uxHighWaterMark);
		   WS2812_Send();


		}

}
void Audio_handler(){
	uint32_t pulNotificationValueaudio;
	while(1){

		xTaskNotifyWait(0,0,&pulNotificationValueaudio,portMAX_DELAY);
	    // Play the audio

		 Audio_play(STOP_audio_data_16bit, STOP_audio_wav_size);
//		  UBaseType_t uxHighWaterMark;
//		  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//		  printf("AudioTask1 unused stack: %lu words\n", uxHighWaterMark);
		 vTaskDelay(1500);

		 Audio_play(STOP_audio_data_16bit, STOP_audio_wav_size);

//		  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//		  printf("AudioTask2 unused stack: %lu words\n", uxHighWaterMark);
		 vTaskDelay(1500);
	}
}
