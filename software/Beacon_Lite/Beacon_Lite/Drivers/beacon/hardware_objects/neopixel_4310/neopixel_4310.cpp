/*
TODO:
- consider a wrapper for HAL_TIM_PWM
- switch to freertos flags
*/
#include "neopixel_4310.h"

#include "main.h"
#include "cpp_main.h"

#include "syscfg.h"

int datasentflag = 0;

namespace neopixel_4310 {
  
Neopixel4310::Neopixel4310() {

}

void Neopixel4310::init() {
  neopixel4310_tim_pwm_done = osEventFlagsNew(NULL);
  uint32_t period = (syscfg::neopixel_4310::kTimerNum.Init.Period) + 1;
  kPwmHigh_ = static_cast<uint8_t>((period * 64) / 100);
  kPwmLow_ = kPwmHigh_ / 2;
  act_blank();

  isInit_ = true;
}

void Neopixel4310::set_colorHSL(beacon_math::HslColor hsl) {
  beacon_math::RgbColor rgb = beacon_math::hslToRgb(hsl);
  uint32_t color = (rgb.r << 16) | (rgb.g << 8) | rgb.b;
  set_pwmData(color);
}

void Neopixel4310::act_blank() {
  constexpr uint32_t blank = 0;
  set_pwmData(blank);
}

void Neopixel4310::act_refresh() {
  HAL_StatusTypeDef status = HAL_OK;
  datasentflag = 0;
  status = HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)pwmData_, kPwmDataSize);
  if (status != HAL_OK) {Error_Handler();}
  osEventFlagsWait(neopixel4310_tim_pwm_done, NEOPIXEL4310_TIM_PWM_DONE, osFlagsWaitAny,
                   osWaitForever);
  while(datasentflag == 0) {
    HAL_Delay(1); 
  }
}

void Neopixel4310::set_pwmData(const uint32_t color) {
  for (int j = 0; j < kNumPixels; j++) {
    for (int i = 0; i < kLedsPerPixel * 8; i++) {
      if (color & (1 << ((kLedsPerPixel * 8 - 1) - i))) {
        pwmData_[j * (kLedsPerPixel * 8) + i] = kPwmHigh_;
      }
      else {
        pwmData_[j * (kLedsPerPixel * 8) + i] = kPwmLow_;
      }
    }
  }
}

void Task_Neopixel4310(void *argument) {
  /*
  TODO: 
    add some logic to check if device is on before refreshing?
    add some logic to check if there is a change to pixel state before refreshing? 
  */
  while(!global_hardware::neopixel4310.get_isInit()) {
    osDelay(10); // Wait for ltr303als to be initialized
  }

  constexpr uint32_t kDelay = 20;
  for(;;) {
    global_hardware::neopixel4310.act_refresh();
    osDelay(kDelay);
  }
}
} // namespace neopixel_4310

