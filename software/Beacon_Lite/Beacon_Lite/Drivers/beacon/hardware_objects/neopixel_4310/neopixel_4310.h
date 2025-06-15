/*
https://www.adafruit.com/product/4310
https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
Currently uses a large array to store the same color for all 96 pixels.a64l
Can use a circular buffer with a CPU interrupt every pixel (more CPU load).
Can use a timer to stop the circular buffer after 96 pixels in the future.

Currently only supports 1 color for all pixels.
*/
#pragma once

#include "stm32h5xx_hal.h"

#include "beacon_math.h"

#ifdef __cplusplus
namespace neopixel_4310 {

extern "C" {
void Task_Neopixel4310(void *argument);
}

namespace {
  constexpr int kNumPixels = 96;
  constexpr int kLedsPerPixel = 3;
  constexpr size_t kPwmDataSize = (kLedsPerPixel * 8) * kNumPixels;
} // namespace

class Neopixel4310 {
 public:
  Neopixel4310();
  void init();

  bool get_isInit() const {return isInit_;};

  void set_colorHSL(beacon_math::HslColor hsl);

  void act_refresh();
  void act_blank();

  friend void Task_Neopixel4310(void *argument);

 private:
  bool isInit_ = false;
  uint8_t kPwmHigh_;
  uint8_t kPwmLow_;

  uint8_t pwmData_[kPwmDataSize] = {0};

  void set_pwmData(const uint32_t color);
};
} // namespace neopixel_4310
#endif // extern "C"