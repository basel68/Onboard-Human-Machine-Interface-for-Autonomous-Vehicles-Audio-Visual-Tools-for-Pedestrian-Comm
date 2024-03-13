/*
 * main_app.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */


#include "main.h"
#include "CS43L22.h"
#include <stdio.h>
#include <math.h>
#include "audio.h"
static void i2c_init();
static void i2s_init();
static void DMA_init();
static void gpio_init();
I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;
 uint16_t* PASS_audio_data_16bit = ( uint16_t*)(&PASS_audio_wav);
 uint16_t* STOP_audio_data_16bit = ( uint16_t*)(&STOP_audio_wav);

 int main (){

	HAL_Init();
	system_clock_config_HSI(SYS_CLOCK_FREQ_144_MHZ);
	printf("main\n");
	gpio_init();
	DMA_init();
	i2c_init();
	i2s_init();
	int status =Audio_init();
	if(status){
		Error_Handler();
	}


//    Audio_play(audio_data_16bit, PASS_audio_wav_size);
//    HAL_Delay(2000);
//    Audio_mute();
//while(1);
while(1){
    // Play the audio

	 Audio_play(PASS_audio_data_16bit, PASS_audio_wav_size);

	 HAL_Delay(1500);

	 Audio_play(STOP_audio_data_16bit, STOP_audio_wav_size);

	 HAL_Delay(1500);
}



//	Audio_play(buffer_audio, 2 * buffer_length);


	return 0;
}
 void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s){



}
void system_clock_config_HSI(uint8_t clock_freq ){
	  RCC_OscInitTypeDef osc_init = {0};
	  RCC_ClkInitTypeDef clk_init = {0};
	  uint32_t FLatency =0;

	  osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	  osc_init.HSIState=RCC_HSI_ON;
	  osc_init.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;
	  osc_init.PLL.PLLState = RCC_PLL_ON;
	  osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	  switch (clock_freq){
	  case (SYS_CLOCK_FREQ_144_MHZ):
		{
		  osc_init.PLL.PLLM=8;
		  osc_init.PLL.PLLN=144;
		  osc_init.PLL.PLLP=2;
		  osc_init.PLL.PLLQ  = 2;
		  clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		  clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		  clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		  FLatency=FLASH_ACR_LATENCY_4WS;
		  break;

		}
	  case (SYS_CLOCK_FREQ_70_MHZ):
		{
		  osc_init.PLL.PLLM=16;
		  osc_init.PLL.PLLN=140;
		  osc_init.PLL.PLLP=2;
		  osc_init.PLL.PLLQ  = 2;
		  clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		  clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		  clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		  FLatency=FLASH_ACR_LATENCY_2WS;
		  break;

		}
	  case (SYS_CLOCK_FREQ_50_MHZ):
		{
		  osc_init.PLL.PLLM=16;
		  osc_init.PLL.PLLN=100;
		  osc_init.PLL.PLLP=2;
		  osc_init.PLL.PLLQ  = 2;
		  clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		  clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		  clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		  FLatency=FLASH_ACR_LATENCY_1WS;
		  break;

		}
	  default:
		  return;
	  }
	 if( HAL_RCC_OscConfig(&osc_init)!= HAL_OK){
		 Error_Handler();
	 }
	 if( HAL_RCC_ClockConfig(&clk_init, FLatency)!= HAL_OK){
			 Error_Handler();
		 }

	 //Systick configuration
	 	 // for 1 ms for every systick interrupt
	 	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	 	// either hclck/8 or hclck so we decided hclk
	 	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


}
static void i2c_init(){

	hi2c1.Instance=I2C1;
	hi2c1.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.ClockSpeed=FREQ_100KHZ;
	hi2c1.Init.DutyCycle=I2C_DUTYCYCLE_2;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	 {
		Error_Handler();
	 }
}
static void i2s_init(){

	  hi2s3.Instance = SPI3;
	  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
	  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
	  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
	  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	  hi2s3.Init.CPOL = I2S_CPOL_LOW;
	  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
	  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    if (HAL_I2S_Init(&hi2s3) != HAL_OK)
	 {
		Error_Handler();
	 }
}
static void gpio_init(){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  /*Configure GPIO pin : Audio_RST_Pin */
	  GPIO_InitStruct.Pin = Audio_RST_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void DMA_init(){

	  /* DMA controller clock enable */
	  __HAL_RCC_DMA1_CLK_ENABLE();

	  /* DMA interrupt init */
	  /* DMA1_Stream5_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void Error_Handler(){
	printf("error occured\n");
	while(1);
}
