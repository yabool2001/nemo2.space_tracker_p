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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "my_rtc.h"
#include "my_conversions.h"
#include "my_rand.h"
#include "astronode_definitions.h"
#include "astronode_application.h"
#include "my_astronode.h"
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
void send_debug_logs ( char* ) ;
void reset_astronode ( void ) ;
void send_astronode_request ( uint8_t* , uint32_t ) ;
bool is_astronode_character_received ( uint8_t* ) ;
bool is_evt_pin_high ( void ) ;
uint32_t get_systick ( void ) ;
bool is_systick_timeout_over ( uint32_t , uint16_t ) ;
void my_tim_init ( TIM_HandleTypeDef ) ;
void my_tim_start ( TIM_HandleTypeDef ) ;
void my_tim_stop ( TIM_HandleTypeDef ) ;

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OPENLOG_RX_Pin GPIO_PIN_11
#define OPENLOG_RX_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define DBG_TX_Pin GPIO_PIN_2
#define DBG_TX_GPIO_Port GPIOA
#define DBG_RX_Pin GPIO_PIN_3
#define DBG_RX_GPIO_Port GPIOA
#define LDG_Pin GPIO_PIN_5
#define LDG_GPIO_Port GPIOA
#define ASTRO_WKUP_Pin GPIO_PIN_0
#define ASTRO_WKUP_GPIO_Port GPIOB
#define ASTRO_RST_Pin GPIO_PIN_1
#define ASTRO_RST_GPIO_Port GPIOB
#define ASTRO_EVT_IT2_Pin GPIO_PIN_2
#define ASTRO_EVT_IT2_GPIO_Port GPIOB
#define ASTRO_EVT_IT2_EXTI_IRQn EXTI2_3_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define OPENLOG_RTS_Pin GPIO_PIN_15
#define OPENLOG_RTS_GPIO_Port GPIOA
#define ASTRO_TX_Pin GPIO_PIN_8
#define ASTRO_TX_GPIO_Port GPIOB
#define ASTRO_RX_Pin GPIO_PIN_9
#define ASTRO_RX_GPIO_Port GPIOB
#define OPENLOG_TX_Pin GPIO_PIN_10
#define OPENLOG_TX_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define FIRMWARE_RELEASE_YEAR			2023
#define HUART_ASTRO						&huart3
#define HUART_DBG						&huart2 // Zrzut na st-link/v3
//#define HUART_DBG						&huart4 // Zrzut na OpenLog SpurkFun
#define UART_TIMEOUT 					1000
#define UART_TX_MAX_BUFF_SIZE			250
#define UART_ASTRO_RX_MAX_BUFF_SIZE		100

#define TIM_SECONDS_THS_SYSTEM_RESET	300		// 300 s = 5 min.
#define MY_ASTRO_INIT_TIME				10
#define ASTRO_LOG_TIMER					60000 	// 60 000 ms = 60 s

#define SECONDS_IN_1_HOUR				3600
#define HAL_STD_TIME_OPS_THS			2		// 2 s

#define HTIM							htim6

// ASTRO
#define ASTRO_PAYLOAD_MAX_LEN			160

//SYSTEM
#define DBG_PAYLOAD_MAX_LEN				250

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
