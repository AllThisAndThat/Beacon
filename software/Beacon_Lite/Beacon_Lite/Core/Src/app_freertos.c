/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cpp_main.h"
// #include "ltr_303als.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Ltr_303als */
osThreadId_t Task_Ltr_303alsHandle;
uint32_t Buffer_Ltr_303ALS[ 216 ];
osStaticThreadDef_t Control_Ltr_303ALS;
const osThreadAttr_t Task_Ltr_303als_attributes = {
  .name = "Task_Ltr_303als",
  .stack_mem = &Buffer_Ltr_303ALS[0],
  .stack_size = sizeof(Buffer_Ltr_303ALS),
  .cb_mem = &Control_Ltr_303ALS,
  .cb_size = sizeof(Control_Ltr_303ALS),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Heartbeat */
osThreadId_t Task_HeartbeatHandle;
uint32_t Buffer_Heartbeat[ 216 ];
osStaticThreadDef_t Control_Heartbeat;
const osThreadAttr_t Task_Heartbeat_attributes = {
  .name = "Task_Heartbeat",
  .stack_mem = &Buffer_Heartbeat[0],
  .stack_size = sizeof(Buffer_Heartbeat),
  .cb_mem = &Control_Heartbeat,
  .cb_size = sizeof(Control_Heartbeat),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Is31fl3741 */
osThreadId_t Task_Is31fl3741Handle;
uint32_t Buffer_Is31fl3741[ 1024 ];
osStaticThreadDef_t Control_Is31fl3741;
const osThreadAttr_t Task_Is31fl3741_attributes = {
  .name = "Task_Is31fl3741",
  .stack_mem = &Buffer_Is31fl3741[0],
  .stack_size = sizeof(Buffer_Is31fl3741),
  .cb_mem = &Control_Is31fl3741,
  .cb_size = sizeof(Control_Is31fl3741),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_AnoEncoder */
osThreadId_t Task_AnoEncoderHandle;
uint32_t Buffer_AnoEncoder[ 256 ];
osStaticThreadDef_t Control_AnoEncoder;
const osThreadAttr_t Task_AnoEncoder_attributes = {
  .name = "Task_AnoEncoder",
  .stack_mem = &Buffer_AnoEncoder[0],
  .stack_size = sizeof(Buffer_AnoEncoder),
  .cb_mem = &Control_AnoEncoder,
  .cb_size = sizeof(Control_AnoEncoder),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Neopixel4310 */
osThreadId_t Task_Neopixel4310Handle;
uint32_t Buffer_Neopixel4310[ 2500 ];
osStaticThreadDef_t Control_Neopixel4310;
const osThreadAttr_t Task_Neopixel4310_attributes = {
  .name = "Task_Neopixel4310",
  .stack_mem = &Buffer_Neopixel4310[0],
  .stack_size = sizeof(Buffer_Neopixel4310),
  .cb_mem = &Control_Neopixel4310,
  .cb_size = sizeof(Control_Neopixel4310),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_ColorSelect */
osThreadId_t Task_ColorSelectHandle;
const osThreadAttr_t Task_ColorSelect_attributes = {
  .name = "Task_ColorSelect",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 1000 * 4
};
/* Definitions for i2c_mutex */
osMutexId_t i2c_mutexHandle;
const osMutexAttr_t i2c_mutex_attributes = {
  .name = "i2c_mutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* creation of i2c_mutex */
  i2c_mutexHandle = osMutexNew(&i2c_mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of Task_Ltr_303als */
  Task_Ltr_303alsHandle = osThreadNew(Task_Ltr_303als, NULL, &Task_Ltr_303als_attributes);

  /* creation of Task_Heartbeat */
  Task_HeartbeatHandle = osThreadNew(Task_Heartbeat, NULL, &Task_Heartbeat_attributes);

  /* creation of Task_Is31fl3741 */
  Task_Is31fl3741Handle = osThreadNew(Task_Is31fl3741, NULL, &Task_Is31fl3741_attributes);

  /* creation of Task_AnoEncoder */
  Task_AnoEncoderHandle = osThreadNew(Task_AnoEncoder, NULL, &Task_AnoEncoder_attributes);

  /* creation of Task_Neopixel4310 */
  Task_Neopixel4310Handle = osThreadNew(Task_Neopixel4310, NULL, &Task_Neopixel4310_attributes);

  /* creation of Task_ColorSelect */
  Task_ColorSelectHandle = osThreadNew(Task_ColorSelect, NULL, &Task_ColorSelect_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_Task_Heartbeat */
/**
* @brief Function implementing the Task_Heartbeat thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Heartbeat */
void Task_Heartbeat(void *argument)
{
  /* USER CODE BEGIN Task_Heartbeat */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
    osDelay(1000);
  }
  /* USER CODE END Task_Heartbeat */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

