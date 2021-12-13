/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define OUT_LED_STAT_Pin GPIO_PIN_13
#define OUT_LED_STAT_GPIO_Port GPIOC
#define OUT_4004_ENABLE_BUS_WRITE_Pin GPIO_PIN_1
#define OUT_4004_ENABLE_BUS_WRITE_GPIO_Port GPIOC
#define OUT_4004_RESET_Pin GPIO_PIN_2
#define OUT_4004_RESET_GPIO_Port GPIOC
#define OUT_4004_D0_Pin GPIO_PIN_0
#define OUT_4004_D0_GPIO_Port GPIOA
#define OUT_4004_D1_Pin GPIO_PIN_1
#define OUT_4004_D1_GPIO_Port GPIOA
#define OUT_4004_D2_Pin GPIO_PIN_2
#define OUT_4004_D2_GPIO_Port GPIOA
#define OUT_4004_D3_Pin GPIO_PIN_3
#define OUT_4004_D3_GPIO_Port GPIOA
#define IN_4004_D3_Pin GPIO_PIN_4
#define IN_4004_D3_GPIO_Port GPIOA
#define IN_4004_D2_Pin GPIO_PIN_5
#define IN_4004_D2_GPIO_Port GPIOA
#define IN_4004_D0_Pin GPIO_PIN_6
#define IN_4004_D0_GPIO_Port GPIOA
#define IN_4004_D1_Pin GPIO_PIN_7
#define IN_4004_D1_GPIO_Port GPIOA
#define IN_4004_CMROM_Pin GPIO_PIN_0
#define IN_4004_CMROM_GPIO_Port GPIOB
#define IN_4004_SYNC_Pin GPIO_PIN_1
#define IN_4004_SYNC_GPIO_Port GPIOB
#define IN_4004_CMRAM0_Pin GPIO_PIN_10
#define IN_4004_CMRAM0_GPIO_Port GPIOB
#define IN_4004_CMRAM1_Pin GPIO_PIN_11
#define IN_4004_CMRAM1_GPIO_Port GPIOB
#define IN_4004_CMRAM3_Pin GPIO_PIN_14
#define IN_4004_CMRAM3_GPIO_Port GPIOB
#define IN_4004_CMRAM2_Pin GPIO_PIN_15
#define IN_4004_CMRAM2_GPIO_Port GPIOB
#define OUT_4004_PHI2_Pin GPIO_PIN_9
#define OUT_4004_PHI2_GPIO_Port GPIOC
#define OUT_4004_PHI1_Pin GPIO_PIN_8
#define OUT_4004_PHI1_GPIO_Port GPIOA
#define OUT_LED_DATA_Pin GPIO_PIN_12
#define OUT_LED_DATA_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
