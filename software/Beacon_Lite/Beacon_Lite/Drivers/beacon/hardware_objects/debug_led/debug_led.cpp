#include "debug_led.h"

#include "syscfg.h"

DebugLed::DebugLed(TIM_HandleTypeDef hTim) {
  hTim_ = hTim;
}

DebugLed::~DebugLed() {

}

HAL_StatusTypeDef DebugLed::set_color(Color color) {
  switch (color) {
    case Color::kRed:
      return set_color(0xFFF, 0, 0);
    case Color::kGreen:
      return set_color(0, 0xFF, 0);
    case Color::kBlue:
      return set_color(0, 0, 0xFFFF);
    case Color::kYellow:
      return set_color(0xFFFF, 0xFF, 0);
    default:
      return HAL_ERROR;
  }
  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::set_color(uint16_t r, uint16_t g, uint16_t b) {
  hTim_.Instance->CCR1 = r;
  hTim_.Instance->CCR2 = g;
  hTim_.Instance->CCR3 = b;
  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::action_start() {
  HAL_StatusTypeDef status;
  status =  HAL_TIM_PWM_Start(&hTim_, TIM_CHANNEL_1);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Start(&hTim_, TIM_CHANNEL_2);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Start(&hTim_, TIM_CHANNEL_3);
  if (status != HAL_OK) {return status;}

  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::action_stop() {
  HAL_StatusTypeDef status;
  status =  HAL_TIM_PWM_Stop(&hTim_, TIM_CHANNEL_1);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Stop(&hTim_, TIM_CHANNEL_2);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Stop(&hTim_, TIM_CHANNEL_3);
  if (status != HAL_OK) {return status;}

  return HAL_OK;
}
