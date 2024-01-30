/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "stdarg.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#define DEBUGMOD

/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
void print_usart2(char *format, ...);


void MX_USART2_UART_Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity);
void MX_USART1_UART_Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity);


/* USER CODE BEGIN Prototypes */
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

