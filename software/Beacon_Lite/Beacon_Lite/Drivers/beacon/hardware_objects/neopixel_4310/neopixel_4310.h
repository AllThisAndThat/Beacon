/*
https://www.adafruit.com/product/4310
Currently uses a large array to store the same color for all 96 pixels.a64l
Can use a circular buffer with a CPU interrupt every pixel (more CPU load).
Can use a timer to stop the circular buffer after 96 pixels in the future.

Currently only supports 1 color for all pixels.
*/

#pragma once

#include "stm32h5xx_hal.h"

constexpr uint8_t kPwmHigh = 26;
constexpr uint8_t kPwmLow = 13;

constexpr int kNumPixels = 96;
constexpr int kLedsPerPixel = 3;
constexpr int kPwmDataSize = (kLedsPerPixel * 8) * kNumPixels;

class Neopixel4310 {
public:
  Neopixel4310();
  ~Neopixel4310();

  void set_colorHSL(uint8_t h, uint8_t s, uint8_t l);
  void set_color(uint32_t color);

  void act_refresh();
  void act_blank();

private:
  uint32_t color_;
  uint8_t pwmData_[kPwmDataSize];
  
  // void initiate();

  void set_pwmData(uint32_t color);
};