/*
 * it.c
 *
 *  Created on: Mar 23, 2024
 *      Author: BASSEL
 *      Interrupts source file for calling irq handlers and interrupt related functions
 */

#include "main.h"
#include "stm32f4xx_it.h"
extern TIM_HandleTypeDef htim8;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern I2S_HandleTypeDef hi2s3;
extern DMA_HandleTypeDef hdma_tim1_ch1;
void DMA2_Stream1_IRQHandler(void)
{
// calling the IRQ handler
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);

}
void TIM8_UP_TIM13_IRQHandler(void)
{

  HAL_TIM_IRQHandler(&htim8);

}
void DMA1_Stream5_IRQHandler(void)
{

  HAL_DMA_IRQHandler(&hdma_spi3_tx);

}

/**
  * @brief This function handles SPI3 global interrupt.
  */
void SPI3_IRQHandler(void)
{

  HAL_I2S_IRQHandler(&hi2s3);

}

