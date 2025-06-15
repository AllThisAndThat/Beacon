#include "cpp_main.h"

#include "cmsis_os2.h"

#include "syscfg.h"


// Global objects
namespace global_hardware {
is31fl3741::IS31FL3741 is31fl3741;
neopixel_4310::Neopixel4310 neopixel4310;
ano_rotary::AnoRotary ano_rotary;
ltr303als::Ltr_303als ltr303als;
} // namespace global_hardware

void cpp_main() {
  

}

// Interrupt event flags
osEventFlagsId_t ltr303als_event_flags;
osEventFlagsId_t ano_rotary_event_flags;
osEventFlagsId_t i2c_dma_done_event_flags;
osEventFlagsId_t neopixel4310_tim_pwm_done;

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == AMB_INT_Pin) {
    osEventFlagsSet(ltr303als_event_flags, LTR303ALS_FLAG_INT);
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == ANO_INT_Pin) {
    osEventFlagsSet(ano_rotary_event_flags, ANO_ROTARY_FLAG_INT);
  }
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance == syscfg::i2c::bus::kIs31fl3741.Instance) {
    osEventFlagsSet(i2c_dma_done_event_flags, I2C_DMA_FLAG_DONE);
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == syscfg::neopixel_4310::kTimerNum.Instance) {
    // Need to reset duty to zero to prevent glitching.
    htim->Instance->CCR1 = 0; 
    HAL_TIM_PWM_Stop_DMA(htim, syscfg::neopixel_4310::kChannelNum);
    osEventFlagsSet(i2c_dma_done_event_flags, I2C_DMA_FLAG_DONE);
  }
}