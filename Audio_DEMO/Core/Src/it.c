/*
 * it.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 */
#include "main.h"
extern DMA_HandleTypeDef hdma_spi3_tx;
void DMA1_Stream5_IRQHandler(void)
{

  HAL_DMA_IRQHandler(&hdma_spi3_tx);
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}
