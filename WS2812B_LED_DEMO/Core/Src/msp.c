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

	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= 0x7 << 16; //usage fault, memory fault and bus fault system exceptions

	//3. configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}

/* Init the low level hardware : GPIO, CLOCK, NVIC... */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi){
	GPIO_InitTypeDef GPIO_InitStruct;

	if(hspi->Instance==SPI1){
	//enable port A and SPI peripherals clocks
	 __HAL_RCC_SPI1_CLK_ENABLE();

	 __HAL_RCC_GPIOA_CLK_ENABLE();
	//init clk and MOSI pins

	GPIO_InitStruct.Pin=SPI1_SCK_Pin|SPI1_MOSI_Pin;
	GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate=GPIO_AF5_SPI1;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

	}
}
