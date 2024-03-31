/*
 * main_app.c
 *
 *  Created on: Mar 26, 2024
 *      Author: BASSEL
 */


#include "main.h"

CAN_HandleTypeDef hcan1;

static void CAN1_init();
void CAN1_Tx();
void system_clock_config_HSI(uint8_t clock_freq);
void CAN_Filter_Config(void);
int main (){

	HAL_Init();// HAL layer initialization

	system_clock_config_HSI(SYS_CLOCK_FREQ_72_MHZ);//configuring clock at 72MHZ

	CAN1_init();

	if( HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY|CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_BUSOFF)!= HAL_OK){
		Error_Handler();

	}

	 CAN_Filter_Config();

	if( HAL_CAN_Start(&hcan1)!= HAL_OK){

		Error_Handler();

	}



		 CAN1_Tx();
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

	 //Systick configuration
	 	 // for 1 ms for every systick interrupt
	 	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	 	// either hclck/8 or hclck so we decided hclk
	 	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


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
void CAN1_Tx()
{
	CAN_TxHeaderTypeDef TxHeader;

	uint32_t TxMailbox;

	uint8_t message;

	TxHeader.DLC = 1;
	TxHeader.StdId = 0x65D;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;


	message = 22;




	if( HAL_CAN_AddTxMessage(&hcan1,&TxHeader,&message,&TxMailbox) != HAL_OK)
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
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{

	printf("Message Transmitted");


}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{

	printf("Message Transmitted");
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	printf("Message Transmitted");

}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

		CAN_RxHeaderTypeDef rxheader;
		uint8_t message;
		if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxheader, &message)!= HAL_OK){
			Error_Handler();
		}
		printf("Message Received:%d\n",message);

}

void Error_Handler(){
	printf("error occured\n");
	while(1);
}
