/*
 * main_app.c
 *
 *  Created on: Mar 23, 2024
 *      Author: BASSEL
 */


#include "main.h"

CAN_HandleTypeDef hcan1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim1_ch1;
uint8_t datasentflag=0;
I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;
state_t curr_state=IDLE;
uint8_t pedestrian_detected=0;



uint16_t start_time_ISR=0;
uint16_t start_time_ISR_big=0;
uint16_t start_time_LED=0;
uint16_t start_time_LED_big=0;
uint16_t start_time_AUDIO_big=0;
uint16_t start_time_SLEEP=0;

uint16_t end_time_SLEEP=0;
uint16_t end_time_ISR=0;
uint16_t end_time_LOOP=0;



static void TIM3_Init(void);
static void i2c_init();
static void i2s_init();
static void DMA1_init();
static void CAN1_init();
static void Audio_RST_gpio_init();
static void TIM1_Init(void);
static void TIM2_Init(void);
static void DMA2_Init(void);
void CAN_Filter_Config(void);
void system_clock_config_HSI(uint8_t clock_freq);
int main (){

	HAL_Init();// HAL layer initialization

	system_clock_config_HSI(SYS_CLOCK_FREQ_72_MHZ);//configuring clock at 72MHZ

	DMA1_init();//DMA1 initialization for audio data
	DMA2_Init(); //DMA2 initialization for led  data

	TIM1_Init(); //TIMER1 initialization
	TIM2_Init();//TIMER2 initialization
	TIM3_Init();//TIMER3 initialization
	i2c_init();//i2c initialization
	i2s_init();//i2s initialization
	Audio_RST_gpio_init();//reset pin of audio codec initialization
	Audio_init();


	CAN1_init();

	if( HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY|CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_BUSOFF)!= HAL_OK){
		Error_Handler();

	}

	 CAN_Filter_Config();

	if( HAL_CAN_Start(&hcan1)!= HAL_OK){

		Error_Handler();

	}

	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim3);


	  while(1){
		  if(pedestrian_detected==PED_DETECTED){

			  led_strip_audio_handler();

			  pedestrian_detected=0;

		  }
		  else{

		  }
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
  * generating a timer to use in evaluating performance
  */
static void TIM2_Init(void)
{


  /**
    * Basic timer initialization
    */

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 110;//clock time period of 1.54167 microseconds (µs)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 64865-1;// to Generate a period of 100 ms
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.RepetitionCounter = 0;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
}
static void TIM3_Init(void)
{


  /**
    * Basic timer initialization
    */

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 11000;//clock time period of 152.792 microseconds (µs)
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65449-1;// to Generate a period of 10 s
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.RepetitionCounter = 0;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
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
/**
  * @brief Can initialization
  */
static void CAN1_init(void)
{
	//500 kbit/s achieved with 18 time quantas
	//clock rate that reaches the can is 36 MHZ
	  hcan1.Instance = CAN1;
	  hcan1.Init.Prescaler = 4;
	  hcan1.Init.Mode = CAN_MODE_NORMAL;
	  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;//sync
	  hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;//prop + phase 1
	  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;//phase 2
	  hcan1.Init.TimeTriggeredMode = DISABLE;
	  hcan1.Init.AutoBusOff = DISABLE;
	  hcan1.Init.AutoWakeUp = DISABLE;
	  hcan1.Init.AutoRetransmission = DISABLE;
	  hcan1.Init.ReceiveFifoLocked = DISABLE;
	  hcan1.Init.TransmitFifoPriority = DISABLE;
	  if (HAL_CAN_Init(&hcan1) != HAL_OK)
	  {
	    Error_Handler();
	  }



}
void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef can1_filter_init;

	can1_filter_init.FilterActivation = ENABLE;
	can1_filter_init.FilterBank  = 0;
	can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
	can1_filter_init.FilterIdHigh = 0x0000;
	can1_filter_init.FilterIdLow = 0x0000;
	can1_filter_init.FilterMaskIdHigh = 0x0000;
	can1_filter_init.FilterMaskIdLow = 0x0000;
	can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	if( HAL_CAN_ConfigFilter(&hcan1,&can1_filter_init) != HAL_OK)
	{
		Error_Handler();
	}

}
void vApplicationIdleHook(){
	//whenever the idle task is running change the state to IDLE
	if(curr_state!=IDLE){
		curr_state=IDLE;
	}

}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

		CAN_RxHeaderTypeDef rxheader;
		uint8_t message;
		start_time_ISR=__HAL_TIM_GET_COUNTER(&htim2);
		start_time_ISR_big=__HAL_TIM_GET_COUNTER(&htim3);
		if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxheader, &message)!= HAL_OK){
			Error_Handler();
		}
		else if(message==PED_DETECTED){
			pedestrian_detected=1;
		}
		 end_time_ISR=__HAL_TIM_GET_COUNTER(&htim2);

}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	/**
	  * stopping the dma to neglect the 5 extra bits the dma delivers
	  */
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	datasentflag=1;
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//
//  if (htim->Instance == TIM8) {
//    HAL_IncTick();
//  }
//
//}
void Error_Handler(){
	printf("error occured\n");
	while(1);
}
