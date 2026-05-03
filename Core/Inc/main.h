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
#include "stm32g4xx_hal.h"

#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_dma.h"

#include "stm32g4xx_ll_exti.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NRST_Pin GPIO_PIN_10
#define NRST_GPIO_Port GPIOG
#define Servo_Uart_RX_Pin GPIO_PIN_0
#define Servo_Uart_RX_GPIO_Port GPIOC
#define Servo_Uart_TX_Pin GPIO_PIN_1
#define Servo_Uart_TX_GPIO_Port GPIOC
#define Latch_Status_GPIO_Input_Pin GPIO_PIN_2
#define Latch_Status_GPIO_Input_GPIO_Port GPIOC
#define Latch_Resent_GPIO_Output_Pin GPIO_PIN_3
#define Latch_Resent_GPIO_Output_GPIO_Port GPIOC
#define Servo_Enable_GPIO_Output_Pin GPIO_PIN_1
#define Servo_Enable_GPIO_Output_GPIO_Port GPIOA
#define UART2_C1_TX_Pin GPIO_PIN_2
#define UART2_C1_TX_GPIO_Port GPIOA
#define UART2_C1_RX_Pin GPIO_PIN_3
#define UART2_C1_RX_GPIO_Port GPIOA
#define SERVO_ADC_Pin GPIO_PIN_5
#define SERVO_ADC_GPIO_Port GPIOA
#define Servo_PWM_Pin GPIO_PIN_6
#define Servo_PWM_GPIO_Port GPIOA
#define C2_Enable_Pin GPIO_PIN_7
#define C2_Enable_GPIO_Port GPIOA
#define UART1_C2_TX_Pin GPIO_PIN_4
#define UART1_C2_TX_GPIO_Port GPIOC
#define UART1_CS_RX_Pin GPIO_PIN_5
#define UART1_CS_RX_GPIO_Port GPIOC
#define C3_Enable_Pin GPIO_PIN_9
#define C3_Enable_GPIO_Port GPIOE
#define Camera_Select_2_Pin GPIO_PIN_11
#define Camera_Select_2_GPIO_Port GPIOE
#define Camera_Select_1_Pin GPIO_PIN_12
#define Camera_Select_1_GPIO_Port GPIOE
#define Video_Enable_Pin GPIO_PIN_13
#define Video_Enable_GPIO_Port GPIOE
#define UART3_C3_RX_Pin GPIO_PIN_15
#define UART3_C3_RX_GPIO_Port GPIOE
#define UART3_C3_TX_Pin GPIO_PIN_10
#define UART3_C3_TX_GPIO_Port GPIOB
#define OSD_Signal_Pin GPIO_PIN_11
#define OSD_Signal_GPIO_Port GPIOB
#define Servo_UART_DE_Pin GPIO_PIN_12
#define Servo_UART_DE_GPIO_Port GPIOB
#define SPI2_SCK_OSD_Pin GPIO_PIN_13
#define SPI2_SCK_OSD_GPIO_Port GPIOB
#define SPI2_MISO_OSD_Pin GPIO_PIN_14
#define SPI2_MISO_OSD_GPIO_Port GPIOB
#define SPI2_MOSI_OSD_Pin GPIO_PIN_15
#define SPI2_MOSI_OSD_GPIO_Port GPIOB
#define OSD_CS_Pin GPIO_PIN_8
#define OSD_CS_GPIO_Port GPIOD
#define VideoTX_Enable_Pin GPIO_PIN_9
#define VideoTX_Enable_GPIO_Port GPIOD
#define OSD_Reset_Pin GPIO_PIN_10
#define OSD_Reset_GPIO_Port GPIOD
#define C1_Enable_Pin GPIO_PIN_9
#define C1_Enable_GPIO_Port GPIOA
#define CAN_Standby_Pin GPIO_PIN_10
#define CAN_Standby_GPIO_Port GPIOA
#define UART5_HD_VTX_Pin GPIO_PIN_12
#define UART5_HD_VTX_GPIO_Port GPIOC
#define MUX_Shutoff_Pin GPIO_PIN_2
#define MUX_Shutoff_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
