#include "cpp_main.h"

#include "cmsis_os2.h"

#include "syscfg.h"
#include "neopixel_4310.h"

// osEventFlagsId_t neopixel4310_tim_pwm_done;
beacon_math::HslColor hsl = {0, 255, 5};
void cpp_main() {
  // neopixel4310_tim_pwm_done = osEventFlagsNew(NULL);
  neopixel_4310::Neopixel4310 led_strip;
  constexpr uint32_t delay = 50;

  for(;;) {
    led_strip.set_colorHSL(hsl);
    led_strip.act_refresh();
    hsl.h += 1;
    HAL_Delay(delay);
  }
}

osEventFlagsId_t ltr303als_event_flags;
osEventFlagsId_t ano_rotary_event_flags;
osEventFlagsId_t i2c_dma_done_event_flags;

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

// volatile int i2c_dma_done;
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance == syscfg::i2c::bus::kIs31fl3741.Instance) {
    osEventFlagsSet(i2c_dma_done_event_flags, I2C_DMA_FLAG_DONE);
  }
}