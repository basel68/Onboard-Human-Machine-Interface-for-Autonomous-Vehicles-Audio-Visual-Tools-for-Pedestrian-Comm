/*
 * main_app.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */


#include "main.h"
static void spi_init();
SPI_HandleTypeDef hspi1;
led_struct_handle_t led_handle;
int main (){

	HAL_Init();
	system_clock_config_HSI(SYS_CLOCK_FREQ_70_MHZ);
	printf("hello\n");

	spi_init();
	led_handle.color=BLUE;
	led_handle.led_number=15;
	while(1){
		WS2812B_send(&led_handle);
	}

	return 0;
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
	  case (SYS_CLOCK_FREQ_80_MHZ):
		{
		  osc_init.PLL.PLLM=16;
		  osc_init.PLL.PLLN=160;
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
static void spi_init(){
	  hspi1.Instance = SPI1;
	  hspi1.Init.Mode = SPI_MODE_MASTER;
	  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
	  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	  hspi1.Init.NSS = SPI_NSS_SOFT;
	  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi1.Init.CRCPolynomial = 10;
	  if (HAL_SPI_Init(&hspi1) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void Error_Handler(){
	printf("error occured\n");
	while(1);
}
