/*
https://lumissil.com/assets/pdf/core/IS31FL3741A_DS.pdf
https://www.adafruit.com/product/5201

TODO:
- Use DMA to transfer data over I2C. Refresh loop.
- Use HSL to RGB conversion
*/
#pragma once

#include "stm32h5xx_hal.h"

#include "i2c_master.h"

enum class Page: uint8_t {
// Page 9 datasheet
  k0 = 0,
  k1 = 1,
  k2 = 2,
  k3 = 3,
  k4 = 4
};

enum class IS31FL3741_State : int {
  kOff = 0,
  kOn = 1
};

struct LedConfig {
  uint32_t x;
  uint32_t y;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// constexpr int kNumRows = 9;
// constexpr int kNumCols = 13;
// constexpr int kNumPixels = kNumRows * kNumCols;
// constexpr int kNumLedsPerPixel = 3;
// constexpr int kArraySize = kNumPixels * kNumLedsPerPixel;

class IS31FL3741 {
public:
  IS31FL3741();
  ~IS31FL3741();

  HAL_StatusTypeDef set_globalCurrent(const uint8_t current);
  HAL_StatusTypeDef set_specificLed(uint32_t led_num, 
                                    const uint8_t pwm,
                                    const uint8_t scale);
  HAL_StatusTypeDef set_pixel(LedConfig config);

  HAL_StatusTypeDef act_verify();
  HAL_StatusTypeDef act_off();
  HAL_StatusTypeDef act_on();
  HAL_StatusTypeDef act_resetAllLeds();

private:
  I2c hI2c_;
  Page page_;
  IS31FL3741_State state_;
  uint8_t global_current_;
  // uint8_t led_data_[kArraySize];

  HAL_StatusTypeDef initiate();

  HAL_StatusTypeDef setup_configRegister();
  HAL_StatusTypeDef setup_pullResistor();

  HAL_StatusTypeDef set_page(const Page page);

  HAL_StatusTypeDef act_writePage(Page page, uint8_t* data,
                                  const size_t data_len);

  void error_handler();
};