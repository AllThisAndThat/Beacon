#include "cpp_main.h"

#include "main.h"

#include "debug_led.h"
#include "i2c_master.h"


void cpp_main(void)
{

  I2C_HandleTypeDef hi2c1;
  TIM_HandleTypeDef htim1;
  
  HAL_Init();
  
  DebugLed debugLed(htim1);
  debugLed.initiate();
  debugLed.action_start();

  debugLed.set_color(Color::kBlue); // Blue
  HAL_Delay(300);

  debugLed.set_color(Color::kGreen); // Green
  HAL_Delay(300);

  debugLed.set_color(Color::kRed); // Red
  HAL_Delay(300);

  constexpr uint16_t dev_address = 0x29 << 1;
  I2cDevice i2c(hi2c1, dev_address);

  uint8_t data;
  HAL_StatusTypeDef status = HAL_OK;
  constexpr uint8_t rGainSetting = 0x80;

  status = i2c.act_pollRead(rGainSetting, &data);
  if (status != HAL_OK) {
    Error_Handler();
  }

  constexpr uint8_t kMaxGainSetting = 0b000'111'0'1; // 96X gain, activates sensor
  status = i2c.act_pollWrite(rGainSetting, kMaxGainSetting);
  if (status != HAL_OK) {
    Error_Handler();
  }
  
  status = i2c.act_pollRead(rGainSetting, &data);
  if (status != HAL_OK) {
    Error_Handler();
  }
  
  if (data == kMaxGainSetting) {
    debugLed.set_color(Color::kGreen);
  }
  else {
    Error_Handler();
  }

  while (1)
  {
    
    // status = i2c.act_pollRead(mem_address, &data);
    // if (status != HAL_OK) {
    //   debugLed.set_color(Color::kBlue);
    //   data = 0;
    // }
    // else if (data == 0xA0) {
    //   debugLed.set_color(Color::kGreen);
    // }
    // else {
    //   debugLed.set_color(Color::kRed);
    // }
    
    HAL_Delay(1);
  }
}