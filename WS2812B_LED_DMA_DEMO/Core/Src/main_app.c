/*
 * main_app.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */


#include "main.h"

TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_tim1_ch1;
uint8_t datasentflag=0;
void system_clock_config_HSI(uint8_t clock_freq);
static void TIM1_Init(void);
static void DMA_Init(void);
int main (){

	HAL_Init();// HAL layer initialization

	system_clock_config_HSI(SYS_CLOCK_FREQ_72_MHZ);//configuring clock at 72MHZ

	DMA_Init(); //DMA initialization

	TIM1_Init(); //TIMER1 initialization



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

	   WS2812_Send();

	   for(int i=0;i<20;i++){
		   Set_LED(i, 0, 255,0);
		  }
		  WS2812_Send();
		  HAL_Delay(1000);
	}

	return 0;
}
/**
  * @brief  System clock Configuration using the HSI
  * @param  clock frequency needed for the System
  */
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
	  case (SYS_CLOCK_FREQ_72_MHZ):
		{
		  osc_init.PLL.PLLM=16;
		  osc_init.PLL.PLLN=144;
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
/**
  * @brief timer initialization
  * generating an OUTPUT CHANNEL for the pwm signal with preiod of 90 to achieve 1.25us as req. for the led strip bit timing
  */
static void TIM1_Init(void)
{

  TIM_OC_InitTypeDef sConfigOC = {0};
  /**
    * Basic timer initialization
    */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 90-1;// to Generate a period of 1.25 us as required by the datasheet
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  /**
    * output channel configurations
    */
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }



}
/**
  * @brief Enable DMA controller clock
  */
static void DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	/**
	  * stopping the dma to neglect the 5 extra bits the dma delivers
	  */
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	datasentflag=1;
}

void Error_Handler(){
	printf("error occured\n");
	while(1);
}
