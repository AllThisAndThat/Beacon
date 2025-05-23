/*
https://lumissil.com/assets/pdf/core/IS31FL3741A_DS.pdf
https://www.adafruit.com/product/5201

TODO:
- Use DMA to transfer data over I2C. Refresh loop.
- Use HSL to RGB conversion
- set region function where you define a region
- explore idea of 2d array.
- add color correction
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

struct HslColor {
  uint8_t h;
  uint8_t s;
  uint8_t l;
};

struct RgbColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

constexpr int kNumRows = 9;
constexpr int kNumCols = 13;
constexpr int kNumPixels = kNumRows * kNumCols;
constexpr int kNumLedsPerPixel = 3;
constexpr int kArraySize = kNumPixels * kNumLedsPerPixel;

class IS31FL3741 {
public:
  IS31FL3741();
  ~IS31FL3741();

  HAL_StatusTypeDef set_globalCurrent(const uint8_t current);

  // HAL_StatusTypeDef set_specificLed(LedConfig config);
  HAL_StatusTypeDef set_pixel(uint32_t row, uint32_t col, HslColor hsl);
  // HAL_StatusTypeDef set_row(uint32_t row, HslColor hsl);
  // HAL_StatusTypeDef set_col(uint32_t col, HslColor hsl);
  HAL_StatusTypeDef set_all(HslColor hsl);


  HAL_StatusTypeDef act_verify();
  HAL_StatusTypeDef act_off();
  HAL_StatusTypeDef act_on();
  HAL_StatusTypeDef act_resetAllLeds();
  HAL_StatusTypeDef act_refreshBrightness();
  HAL_StatusTypeDef act_refreshColor();


private:
  I2c hI2c_;
  Page page_;
  IS31FL3741_State state_;
  uint8_t global_current_;
  uint8_t led_data_[kArraySize] = {0};

  HAL_StatusTypeDef initiate();

  HAL_StatusTypeDef setup_configRegister();
  HAL_StatusTypeDef setup_pullResistor();

  HAL_StatusTypeDef set_page(const Page page);

  HAL_StatusTypeDef act_writePage(Page page, uint8_t* data,
                                  const size_t data_len);
  uint32_t act_mapRow(const uint32_t row);
  RgbColor act_hslToRgb(HslColor hsl);

  void error_handler();
};