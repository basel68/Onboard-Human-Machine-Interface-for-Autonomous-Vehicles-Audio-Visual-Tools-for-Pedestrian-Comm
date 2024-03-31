/*
 * it.c
 *
 *  Created on: Mar 5, 2024
 *      Author: BASSEL
 *      Interrupts source file for calling irq handlers and interrupt related functions
 */

#include "main.h"
#include "stm32f4xx_it.h"
extern CAN_HandleTypeDef hcan1;

void CAN1_RX0_IRQHandler(void){

	HAL_CAN_IRQHandler(&hcan1);
}
void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}
void SysTick_Handler(void)
{

  HAL_IncTick();
}
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

