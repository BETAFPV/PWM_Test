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
#define TF_CS_Pin GPIO_PIN_14
#define TF_CS_GPIO_Port GPIOC
#define KeyA_Pin GPIO_PIN_0
#define KeyA_GPIO_Port GPIOC
#define KeyB_Pin GPIO_PIN_1
#define KeyB_GPIO_Port GPIOC
#define KeyC_Pin GPIO_PIN_2
#define KeyC_GPIO_Port GPIOC
#define KeyD_Pin GPIO_PIN_3
#define KeyD_GPIO_Port GPIOC
#define SPI_CS_Pin GPIO_PIN_4
#define SPI_CS_GPIO_Port GPIOA
#define KeyE_Pin GPIO_PIN_4
#define KeyE_GPIO_Port GPIOC
#define LED_SYS_Pin GPIO_PIN_5
#define LED_SYS_GPIO_Port GPIOC
#define LCD_RES_Pin GPIO_PIN_0
#define LCD_RES_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOB
#define KEY_2_Pin GPIO_PIN_12
#define KEY_2_GPIO_Port GPIOB
#define KEY_1_Pin GPIO_PIN_13
#define KEY_1_GPIO_Port GPIOB
#define BUZZ_Pin GPIO_PIN_10
#define BUZZ_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#include <stdint.h>  // 包含 int32_t、uint32_t 等类型的定义

// 有符号整数类型别名
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

// 有符号常量整数类型别名
typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

// 带 _IO 属性的有符号整数类型别名（通常用于可读写的 IO 操作）
typedef  int32_t vs32;
typedef  int16_t vs16;
typedef  int8_t vs8;

// 带 _I 属性的有符号整数类型别名（通常用于只读的输入操作）
typedef  int32_t vsc32;
typedef  int16_t vsc16;
typedef  int8_t vsc8;

// 无符号整数类型别名
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// 无符号常量整数类型别名（补充常见定义）
typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

// 带 _IO 属性的无符号整数类型别名（补充常见定义）
typedef  uint32_t vu32;
typedef  uint16_t vu16;
typedef  uint8_t vu8;

// 带 _I 属性的无符号整数类型别名（补充常见定义）
typedef  uint32_t vuc32;
typedef  uint16_t vuc16;
typedef  uint8_t vuc8;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
