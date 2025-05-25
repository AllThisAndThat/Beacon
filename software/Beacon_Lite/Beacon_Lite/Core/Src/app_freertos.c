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
osStaticThreadDef_t control_Is31fl3741;
const osThreadAttr_t Task_Is31fl3741_attributes = {
  .name = "Task_Is31fl3741",
  .stack_mem = &Buffer_Is31fl3741[0],
  .stack_size = sizeof(Buffer_Is31fl3741),
  .cb_mem = &control_Is31fl3741,
  .cb_size = sizeof(control_Is31fl3741),
  .priority = (osPriority_t) osPriorityNormal,
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
  // cpp_main();
  /* USER CODE END Init */

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

