/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_manager.h"
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PDM_AN_IN1_Pin GPIO_PIN_0
#define PDM_AN_IN1_GPIO_Port GPIOC
#define PDM_AN_IN2_Pin GPIO_PIN_1
#define PDM_AN_IN2_GPIO_Port GPIOC
#define PDM_AN_IN3_Pin GPIO_PIN_2
#define PDM_AN_IN3_GPIO_Port GPIOC
#define PDM_AN_IN4_Pin GPIO_PIN_3
#define PDM_AN_IN4_GPIO_Port GPIOC
#define PDM_IS_L1_2_Pin GPIO_PIN_0
#define PDM_IS_L1_2_GPIO_Port GPIOA
#define PDM_IS_L3_4_Pin GPIO_PIN_1
#define PDM_IS_L3_4_GPIO_Port GPIOA
#define PDM_IS_M2_Pin GPIO_PIN_2
#define PDM_IS_M2_GPIO_Port GPIOA
#define PDM_BATTERY_SENSE_Pin GPIO_PIN_3
#define PDM_BATTERY_SENSE_GPIO_Port GPIOA
#define PDM_IS_M1_Pin GPIO_PIN_4
#define PDM_IS_M1_GPIO_Port GPIOA
#define PDM_IS_H3_Pin GPIO_PIN_5
#define PDM_IS_H3_GPIO_Port GPIOA
#define PDM_IS_H2_Pin GPIO_PIN_6
#define PDM_IS_H2_GPIO_Port GPIOA
#define PDM_IS_H1_Pin GPIO_PIN_7
#define PDM_IS_H1_GPIO_Port GPIOA
#define PDM_L_OUT2_EN_Pin GPIO_PIN_0
#define PDM_L_OUT2_EN_GPIO_Port GPIOB
#define PDM_L_DSEL1_2_Pin GPIO_PIN_1
#define PDM_L_DSEL1_2_GPIO_Port GPIOB
#define PDM_L_OUT1_EN_Pin GPIO_PIN_2
#define PDM_L_OUT1_EN_GPIO_Port GPIOB
#define PDM_L_OUT3_EN_Pin GPIO_PIN_10
#define PDM_L_OUT3_EN_GPIO_Port GPIOB
#define PDM_LED_CAN_TX_Pin GPIO_PIN_12
#define PDM_LED_CAN_TX_GPIO_Port GPIOB
#define PDM_LED_CAN_RX_Pin GPIO_PIN_13
#define PDM_LED_CAN_RX_GPIO_Port GPIOB
#define PDM_H_OUT1_EN_Pin GPIO_PIN_14
#define PDM_H_OUT1_EN_GPIO_Port GPIOB
#define PDM_H_OUT2_EN_Pin GPIO_PIN_15
#define PDM_H_OUT2_EN_GPIO_Port GPIOB
#define PDM_H_OUT3_EN_Pin GPIO_PIN_6
#define PDM_H_OUT3_EN_GPIO_Port GPIOC
#define PDM_M_OUT1_EN_Pin GPIO_PIN_7
#define PDM_M_OUT1_EN_GPIO_Port GPIOC
#define PDM_M_OUT2_EN_Pin GPIO_PIN_8
#define PDM_M_OUT2_EN_GPIO_Port GPIOC
#define PDM_L_OUT4_EN_Pin GPIO_PIN_9
#define PDM_L_OUT4_EN_GPIO_Port GPIOC
#define PDM_L_DSEL3_4_Pin GPIO_PIN_8
#define PDM_L_DSEL3_4_GPIO_Port GPIOA
#define PDM_DIG_IN1_Pin GPIO_PIN_15
#define PDM_DIG_IN1_GPIO_Port GPIOA
#define PDM_DIG_IN2_Pin GPIO_PIN_10
#define PDM_DIG_IN2_GPIO_Port GPIOC
#define PDM_DIG_IN3_Pin GPIO_PIN_11
#define PDM_DIG_IN3_GPIO_Port GPIOC
#define PDM_DIG_IN4_Pin GPIO_PIN_12
#define PDM_DIG_IN4_GPIO_Port GPIOC
#define PDM_STATUS_Pin GPIO_PIN_2
#define PDM_STATUS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
