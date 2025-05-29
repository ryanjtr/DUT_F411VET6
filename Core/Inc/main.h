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
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern I2C_HandleTypeDef hi2c2;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI2_CSS_Pin LL_GPIO_PIN_2
#define SPI2_CSS_GPIO_Port GPIOA
#define TRIGGER_ADC_SINE_Pin LL_GPIO_PIN_4
#define TRIGGER_ADC_SINE_GPIO_Port GPIOC
#define TRIGGER_ADC_SINE_EXTI_IRQn EXTI4_IRQn
#define CONTROL_REDLED_SOFTWARE_Pin LL_GPIO_PIN_8
#define CONTROL_REDLED_SOFTWARE_GPIO_Port GPIOD
#define CONTROL_GREENLED_SOFTWARE_Pin LL_GPIO_PIN_9
#define CONTROL_GREENLED_SOFTWARE_GPIO_Port GPIOD
#define CONTROL_BLUELED_SOFTWARE_Pin LL_GPIO_PIN_10
#define CONTROL_BLUELED_SOFTWARE_GPIO_Port GPIOD
#define GREEN_LED_Pin LL_GPIO_PIN_12
#define GREEN_LED_GPIO_Port GPIOD
#define ORANGE_LED_Pin LL_GPIO_PIN_13
#define ORANGE_LED_GPIO_Port GPIOD
#define BLUE_LED_Pin LL_GPIO_PIN_15
#define BLUE_LED_GPIO_Port GPIOD
#define SW6_MMI_Pin LL_GPIO_PIN_2
#define SW6_MMI_GPIO_Port GPIOD
#define SW6_MMI_EXTI_IRQn EXTI2_IRQn
#define TAKE_TRIGGER_RUN_FROM_HIL_Pin LL_GPIO_PIN_6
#define TAKE_TRIGGER_RUN_FROM_HIL_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
