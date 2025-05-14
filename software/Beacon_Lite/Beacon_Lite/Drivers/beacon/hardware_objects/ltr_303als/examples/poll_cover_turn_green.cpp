#include "cpp_main.h"

#include "debug_led.h"
#include "ltr_303als.h"

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

  debugLed.set_color(Color::kGreen); // Green

  uint32_t brightness;
  while (1)
  {
    ltr303als.act_pollBrightness(&brightness);
    if (brightness > 0x3E8) {
      debugLed.set_color(Color::kBlue);
    } 
    else {
      debugLed.set_color(Color::kGreen);
    }
    
    HAL_Delay(1);
  }
}