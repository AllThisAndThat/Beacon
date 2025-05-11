
#include <stdio.h>

#include "stm32h5xx_hal.h"

#include "main.h"


extern "C" void app_main() {

  
  while (1)
  {
    HAL_Delay(500);
  }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == NUCLEO_BUTTON_Pin){
	  HAL_GPIO_TogglePin(NUCLEO_LED_GPIO_Port, NUCLEO_LED_Pin);
  }
  else {
  __NOP();
  }
}
