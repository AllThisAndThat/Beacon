#include "cpp_main.h"

#include "stm32h5xx_hal.h"

#include "debug_led.h"
#include "ltr_303als.h"

enum class Flag
{
  kReady,
  kInt
};

volatile Flag amb_int = Flag::kReady;

void cpp_main(handles_t handles)
{
  TIM_HandleTypeDef htim1 = *(handles.htim);
  I2C_HandleTypeDef hi2c1 = *(handles.hi2c);
  
  DebugLed debugLed(htim1);
  debugLed.action_start();

  debugLed.set_color(Color::kBlue); // Blue
  HAL_Delay(300);
  debugLed.set_color(Color::kGreen); // Green
  HAL_Delay(300);
  debugLed.set_color(Color::kRed); // Red
  HAL_Delay(300);


  Ltr_303als ltr303als(hi2c1);

  debugLed.set_color(Color::kGreen);
  HAL_StatusTypeDef status;
  while (1)
  {
    if (amb_int == Flag::kInt) {
      amb_int = Flag::kReady;
      status = ltr303als.act_setInterruptThresholds();
      if (status != HAL_OK) { Error_Handler();}
      debugLed.set_color(Color::kYellow);
      HAL_Delay(100);
      debugLed.set_color(Color::kGreen); 
      HAL_Delay(100);
    }
    else {
      debugLed.set_color(Color::kBlue);
    }
    HAL_Delay(10);
    // ltr303als.get_brightness(&brightness);
  }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == AMB_INT_Pin) {
	  amb_int = Flag::kInt;
  }
}