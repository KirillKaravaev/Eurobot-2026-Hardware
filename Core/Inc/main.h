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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MicroRos.h"
#include "ServoTask.h"
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
#define STATUS_LED_Pin GPIO_PIN_3
#define STATUS_LED_GPIO_Port GPIOE
#define GPIO_RES1_Pin GPIO_PIN_1
#define GPIO_RES1_GPIO_Port GPIOC
#define BAT_VOLTAGE_Pin GPIO_PIN_4
#define BAT_VOLTAGE_GPIO_Port GPIOC
#define START_Pin GPIO_PIN_5
#define START_GPIO_Port GPIOC
#define LOG_UART7_RX_Pin GPIO_PIN_7
#define LOG_UART7_RX_GPIO_Port GPIOE
#define LOG_UART7_TX_Pin GPIO_PIN_8
#define LOG_UART7_TX_GPIO_Port GPIOE
#define JGB37_PWM1_Pin GPIO_PIN_14
#define JGB37_PWM1_GPIO_Port GPIOB
#define JGB37_PWM2_Pin GPIO_PIN_15
#define JGB37_PWM2_GPIO_Port GPIOB
#define JGB37_DIR1_Pin GPIO_PIN_11
#define JGB37_DIR1_GPIO_Port GPIOD
#define JGB37_DIR2_Pin GPIO_PIN_14
#define JGB37_DIR2_GPIO_Port GPIOD
#define JGB37_DIR3_Pin GPIO_PIN_15
#define JGB37_DIR3_GPIO_Port GPIOD
#define JGB37_IMP1_Pin GPIO_PIN_6
#define JGB37_IMP1_GPIO_Port GPIOC
#define JGB37_IMP2_Pin GPIO_PIN_7
#define JGB37_IMP2_GPIO_Port GPIOC
#define JGB37_IMP3_Pin GPIO_PIN_8
#define JGB37_IMP3_GPIO_Port GPIOC
#define JGB37_IMP4_Pin GPIO_PIN_9
#define JGB37_IMP4_GPIO_Port GPIOC
#define JGB37_PWM3_Pin GPIO_PIN_8
#define JGB37_PWM3_GPIO_Port GPIOA
#define JGB37_PWM4_Pin GPIO_PIN_9
#define JGB37_PWM4_GPIO_Port GPIOA
#define JGB37_DIR4_Pin GPIO_PIN_10
#define JGB37_DIR4_GPIO_Port GPIOA
#define GPIO_RES9_Pin GPIO_PIN_6
#define GPIO_RES9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
