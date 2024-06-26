/*
 * main_app.c
 *
 *  Created on: Mar 23, 2024
 *      Author: BASSEL
 */


#include "main.h"

TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_tim1_ch1;
uint8_t datasentflag=0;
I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

static void i2c_init();
static void i2s_init();
static void DMA1_init();
static void Audio_RST_gpio_init();
static void TIM1_Init(void);
static void DMA2_Init(void);
void system_clock_config_HSI(uint8_t clock_freq);
int main (){
	BaseType_t status;
	TaskHandle_t led_Strip_handle;
	TaskHandle_t Audio_handle;

	HAL_Init();// HAL layer initialization

	system_clock_config_HSI(SYS_CLOCK_FREQ_72_MHZ);//configuring clock at 72MHZ

	DMA1_init();//DMA1 initialization for audio data
	DMA2_Init(); //DMA2 initialization for led  data

	TIM1_Init(); //TIMER1 initialization
	i2c_init();//i2c initialization
	i2s_init();//i2s initialization
	Audio_RST_gpio_init();//reset pin of audio codec initialization
	Audio_init();

	//xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask)

	 status = xTaskCreate(led_strip_handler, "LED_strip_task", 200,NULL, 2, &led_Strip_handle);

	 configASSERT(status == pdPASS);

	 status = xTaskCreate(Audio_handler, "Audio_task", 200,NULL, 2, &Audio_handle);

	 configASSERT(status == pdPASS);

	 //start the freeRTOS scheduler
	  vTaskStartScheduler();

	  while(1);

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
		  osc_init.PLL.PLLM=8;
		  osc_init.PLL.PLLN=80;
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
		  osc_init.PLL.PLLM=8;
		  osc_init.PLL.PLLN=72;
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

//	 //Systick configuration
//	 	 // for 1 ms for every systick interrupt
//	 	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
//	 	// either hclck/8 or hclck so we decided hclk
//	 	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


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
static void DMA2_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

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
static void Audio_RST_gpio_init(){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  /*Configure GPIO pin : Audio_RST_Pin */
	  GPIO_InitStruct.Pin = Audio_RST_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void DMA1_init(){

	  /* DMA controller clock enable */
	  __HAL_RCC_DMA1_CLK_ENABLE();

	  /* DMA interrupt init */
	  /* DMA1_Stream5_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	/**
	  * stopping the dma to neglect the 5 extra bits the dma delivers
	  */
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	datasentflag=1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM8) {
    HAL_IncTick();
  }

}
void Error_Handler(){
	printf("error occured\n");
	while(1);
}
