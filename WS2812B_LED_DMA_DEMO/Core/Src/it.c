/*
 * it.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 *      Interrupts source file for calling irq handlers and interrupt related functions
 */

#include "main.h"
#include "stm32f4xx_it.h"
extern DMA_HandleTypeDef hdma_tim1_ch1;
void SysTick_Handler(void)
{

  HAL_IncTick();// increment global tick

}
void DMA2_Stream1_IRQHandler(void)
{
// calling the IRQ handler
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);

}
