/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define MCU_SENS1_Pin GPIO_PIN_0
#define MCU_SENS1_GPIO_Port GPIOC
#define MCU_SENS2_Pin GPIO_PIN_1
#define MCU_SENS2_GPIO_Port GPIOC
#define MCU_SENS3_Pin GPIO_PIN_2
#define MCU_SENS3_GPIO_Port GPIOC
#define MCU_SENS4_Pin GPIO_PIN_3
#define MCU_SENS4_GPIO_Port GPIOC
#define MCU_IS_L1_2_Pin GPIO_PIN_0
#define MCU_IS_L1_2_GPIO_Port GPIOA
#define MCU_IS_L3_4_Pin GPIO_PIN_1
#define MCU_IS_L3_4_GPIO_Port GPIOA
#define MCU_IS_M2_Pin GPIO_PIN_2
#define MCU_IS_M2_GPIO_Port GPIOA
#define MCU_BATT_REF_Pin GPIO_PIN_3
#define MCU_BATT_REF_GPIO_Port GPIOA
#define MCU_IS_M1_Pin GPIO_PIN_4
#define MCU_IS_M1_GPIO_Port GPIOA
#define MCU_IS_H3_Pin GPIO_PIN_5
#define MCU_IS_H3_GPIO_Port GPIOA
#define MCU_IS_H2_Pin GPIO_PIN_6
#define MCU_IS_H2_GPIO_Port GPIOA
#define MCU_IS_H1_Pin GPIO_PIN_7
#define MCU_IS_H1_GPIO_Port GPIOA
#define MCU_IN_L2_Pin GPIO_PIN_0
#define MCU_IN_L2_GPIO_Port GPIOB
#define MCU_DSEL_L1_2_Pin GPIO_PIN_1
#define MCU_DSEL_L1_2_GPIO_Port GPIOB
#define MCU_IN_L1_Pin GPIO_PIN_2
#define MCU_IN_L1_GPIO_Port GPIOB
#define MCU_IN_L3_Pin GPIO_PIN_10
#define MCU_IN_L3_GPIO_Port GPIOB
#define STATUS_CAN_TX_Pin GPIO_PIN_12
#define STATUS_CAN_TX_GPIO_Port GPIOB
#define STATUS_CAN_RX_Pin GPIO_PIN_13
#define STATUS_CAN_RX_GPIO_Port GPIOB
#define MCU_IN_H1_Pin GPIO_PIN_14
#define MCU_IN_H1_GPIO_Port GPIOB
#define MCU_IN_H2_Pin GPIO_PIN_15
#define MCU_IN_H2_GPIO_Port GPIOB
#define MCU_IN_H3_Pin GPIO_PIN_6
#define MCU_IN_H3_GPIO_Port GPIOC
#define MCU_IN_M1_Pin GPIO_PIN_7
#define MCU_IN_M1_GPIO_Port GPIOC
#define MCU_IN_M2_Pin GPIO_PIN_8
#define MCU_IN_M2_GPIO_Port GPIOC
#define MCU_IN_L4_Pin GPIO_PIN_9
#define MCU_IN_L4_GPIO_Port GPIOC
#define MCU_DSEL_L3_4_Pin GPIO_PIN_8
#define MCU_DSEL_L3_4_GPIO_Port GPIOA
#define MCU_DIG1_Pin GPIO_PIN_15
#define MCU_DIG1_GPIO_Port GPIOA
#define MCU_DIG2_Pin GPIO_PIN_10
#define MCU_DIG2_GPIO_Port GPIOC
#define MCU_DIG3_Pin GPIO_PIN_11
#define MCU_DIG3_GPIO_Port GPIOC
#define MCU_DIG4_Pin GPIO_PIN_12
#define MCU_DIG4_GPIO_Port GPIOC
#define LD_STATUS_Pin GPIO_PIN_2
#define LD_STATUS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
