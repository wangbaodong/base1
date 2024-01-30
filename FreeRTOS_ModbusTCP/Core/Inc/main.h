/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "string.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/*
	TASK_BIT_1 ModbusTCPÖ÷º¯Êý¼à¿Ø
	TASK_BIT_4 ÖØÆôº¯Êý¼à¿Ø
*/
#define TASK_BIT_1    (0x01 << 0)
#define TASK_BIT_2    (0x01 << 1)
#define TASK_BIT_3    (0x01 << 2)
#define TASK_BIT_4    (0x01 << 3)
#define TASK_BIT_5    (0x01 << 4)
#define TASK_BIT_6    (0x01 << 5)
#define TASK_BIT_ALL	( TASK_BIT_1|TASK_BIT_4 )
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void sdk_MBCallBack(int value);
int Handle(int value,void (*Callback)(int));
/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW1_Pin GPIO_PIN_10
#define SW1_GPIO_Port GPIOE
#define SW1_EXTI_IRQn EXTI15_10_IRQn
#define SW2_Pin GPIO_PIN_11
#define SW2_GPIO_Port GPIOE
#define SW2_EXTI_IRQn EXTI15_10_IRQn
#define SW3_Pin GPIO_PIN_12
#define SW3_GPIO_Port GPIOE
#define SW3_EXTI_IRQn EXTI15_10_IRQn
#define led1_Pin GPIO_PIN_13
#define led1_GPIO_Port GPIOE
#define led2_Pin GPIO_PIN_14
#define led2_GPIO_Port GPIOE
#define led3_Pin GPIO_PIN_15
#define led3_GPIO_Port GPIOE
#define RS485cs_Pin GPIO_PIN_7
#define RS485cs_GPIO_Port GPIOD

#define LCD_PWR_PORT GPIOE
#define LCD_PWR_PIN		GPIO_PIN_7
#define LCD_RST_PORT GPIOE
#define LCD_RST_PIN		GPIO_PIN_9
#define LCD_WR_PORT GPIOC
#define LCD_WR_PIN		GPIO_PIN_2


#define SPI_CSS_PORT		GPIOE
#define SPI_CSS_PIN			GPIO_PIN_8
#define RE_DE_TX()		 HAL_GPIO_WritePin(RS485cs_GPIO_Port, RS485cs_Pin, GPIO_PIN_SET)
#define RE_DE_RX()		 HAL_GPIO_WritePin(RS485cs_GPIO_Port, RS485cs_Pin, GPIO_PIN_RESET)
/* USER CODE BEGIN Private defines */
void SystemClock_Config(void);
void delayUs(uint32_t nus);
void demo_show_cube(void);
void delayUs2(uint32_t nus);
void logTime(void);
void addError(const char * buff,uint8_t value);
void USART2_IRQHandler(void);
void USART1_IRQHandler(void);
void  MX_FREERTOS_Init(void (*osThreadFunc_t) (void *argument));
void PeriodicLogCallback(void *argument);

typedef struct {
    char errorMessage[30];
} LastErrorTpye;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
