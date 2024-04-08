/*
 * msp.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */
#include "main.h"

//will do low level processor specific inits.
void HAL_MspInit(void)
{

	//1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= 0x7 << 16; //usage fault, memory fault and bus fault system exceptions

	//3. configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hcan->Instance==CAN1)
	 {
	   /* Peripheral clock enable */
	   __HAL_RCC_CAN1_CLK_ENABLE();

	   __HAL_RCC_GPIOD_CLK_ENABLE();
	   /**CAN1 GPIO Configuration
	   PD0     ------> CAN1_RX
	   PD1     ------> CAN1_TX
	   */
	   GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	   GPIO_InitStruct.Pull = GPIO_NOPULL;
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	   GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
	   HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	 }
	//enable can receive interrupt irq handler in arm processor
	HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
}

