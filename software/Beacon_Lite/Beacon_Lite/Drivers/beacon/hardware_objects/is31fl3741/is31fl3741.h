/*
TODO:
- set region function where you define a region
- add color correction
- decouple our physical layout (row/col/remapping) from this library and move to a higher level
*/
#pragma once

#include "stm32h5xx_hal.h"

#include "i2c_device.h"

#ifdef __cplusplus
extern "C" {
#endif

void Task_Is31fl3741(void *argument);

#ifdef __cplusplus
}
#endif

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

namespace {
enum class IS31FL3741_State : int {
  kOff = 0,
  kOn = 1
};

enum class Page: uint8_t {
// Page 9 datasheet
  k0 = 0,
  k1 = 1,
  k2 = 2,
  k3 = 3,
  k4 = 4
};

constexpr size_t kPage0ArraySize = 180;
constexpr size_t kPage1ArraySize = 171;

} // namespace

class IS31FL3741 {
 public:
  IS31FL3741();

  HAL_StatusTypeDef set_globalCurrent(const uint8_t current);
  HAL_StatusTypeDef set_pixel(uint32_t row, uint32_t col, HslColor hsl);
  HAL_StatusTypeDef set_pixel(uint32_t row, uint32_t col, RgbColor rgb);
  HAL_StatusTypeDef set_row(uint32_t row, HslColor hsl);
  HAL_StatusTypeDef set_col(uint32_t col, HslColor hsl);
  void set_all_pixels(HslColor hsl);
  void set_pixels_blank();

  void act_verifyId();
  void act_off();
  void act_on();
  HAL_StatusTypeDef act_refreshBrightness();
  HAL_StatusTypeDef act_refreshColor();

 private:
  Page page_;
  uint8_t page0_leds_[kPage0ArraySize] = {0};
  uint8_t page1_leds_[kPage1ArraySize] = {0};
  I2cDevice hI2c_;

  void setup_configAndDisable();
  void setup_pullResistor();

  void set_page(const Page page);

  RgbColor act_hslToRgb(HslColor hsl);
  void act_swReset();
};