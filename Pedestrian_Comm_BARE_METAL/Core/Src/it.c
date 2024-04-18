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
extern CAN_HandleTypeDef hcan1;

void CAN1_RX0_IRQHandler(void){

	HAL_CAN_IRQHandler(&hcan1);
}
void DMA2_Stream1_IRQHandler(void)
{
// calling the IRQ handler
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);

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
void SysTick_Handler(void)
{

  HAL_IncTick();

}
void NMI_Handler(void)
{

  while (1)
  {
  }

}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

