/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pong.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define L1_Pin GPIO_PIN_1
#define L1_GPIO_Port GPIOB
#define L2_Pin GPIO_PIN_2
#define L2_GPIO_Port GPIOB
#define L3_Pin GPIO_PIN_10
#define L3_GPIO_Port GPIOB
#define L4_Pin GPIO_PIN_11
#define L4_GPIO_Port GPIOB
#define L5_Pin GPIO_PIN_12
#define L5_GPIO_Port GPIOB
#define L6_Pin GPIO_PIN_13
#define L6_GPIO_Port GPIOB
#define L7_Pin GPIO_PIN_14
#define L7_GPIO_Port GPIOB
#define L8_Pin GPIO_PIN_15
#define L8_GPIO_Port GPIOB
#define SPI_CS_Pin GPIO_PIN_8
#define SPI_CS_GPIO_Port GPIOA
#define BTN1_Pin GPIO_PIN_11
#define BTN1_GPIO_Port GPIOA
#define BTN1_EXTI_IRQn EXTI15_10_IRQn
#define BTN2_Pin GPIO_PIN_12
#define BTN2_GPIO_Port GPIOA
#define BTN2_EXTI_IRQn EXTI15_10_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
