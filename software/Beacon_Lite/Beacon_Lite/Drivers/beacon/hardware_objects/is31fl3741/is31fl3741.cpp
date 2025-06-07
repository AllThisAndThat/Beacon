/*
https://lumissil.com/assets/pdf/core/IS31FL3741A_DS.pdf
https://www.adafruit.com/product/5201
*/

#include "is31fl3741.h"

#include <cstring>

#include "cmsis_os2.h"
#include "cpp_main.h"

#include "syscfg.h"


namespace is31fl3741 {
/*
Since setting a pixel is dependent on PCB layout,
we use a function pointer to define how to map pixels.
*/
#if PCB1_LAYOUT
constexpr auto set_pixel_func = &is31fl3741::IS31FL3741::set_pixel;
#elif defined(BREADBOARD)
constexpr auto set_pixel_func = &IS31FL3741::set_pixel_adafruit5201;
#endif

// I2C configuration
namespace I2c {
  constexpr uint16_t kAddr = syscfg::i2c::addr::kIs31fl3741;
  inline I2C_HandleTypeDef&  kBus  = syscfg::i2c::bus::kIs31fl3741;
  constexpr RegSize  kRegSize = RegSize::k8Bit;
} // namespace I2c
// Pageless Registers
/*
Page 10
Read only
Returns the device ID.
*/
constexpr uint16_t rDeviceId = 0xFC;
constexpr uint8_t  kDeviceId = 0x60;
/*
page 10
Write only (if unlocked)
Selects the active page.
*/
constexpr uint8_t rPageSelect = 0xFD;
/*
Page 10
Read/Write
Unlocks page change register once.
*/
constexpr uint16_t rUnlock = 0xFE;
constexpr uint8_t  kUnlock = 0b1100'0101;
// Page 0 Registers - PWM Registers 1
// Page 1 Registers - PWM Registers 2
// Page 2 Registers - Scale Registers 1
// Page 3 Registers - Scale Registers 2
// Page 4 Registers
/*
Page 14
Read/Write
Enables/Disables IC, enables open/short detection,
sets logic voltage, and defines array shape.
*/
constexpr uint16_t rOperationConfig  =  0x00;
constexpr uint8_t kOpenOrShortDetect = (0b00 << 1); // Disabled open and short detection
constexpr uint8_t kDeviceDisabled = syscfg::is31fl3741::kNumChannelScan << 4
                                  | syscfg::is31fl3741::kLogicThresholds << 3
                                  | kOpenOrShortDetect;
constexpr uint8_t kDeviceEnabled = kDeviceDisabled | 0b1;                             
/*
Page 14
Read/Write
Sets the global current for all LEDs. Linear increase for 0x00 to 0xFF.
*/
constexpr uint16_t rGlobalCurrent = 0x01;
/*
Page 15
Read/Write
Assigns pull-up and pull-down when a channel is inactive. 
Drains residual charge from led (ghosting).
*/
constexpr uint16_t rPullResistor  =  0x02;
constexpr uint8_t kPullResistor = syscfg::is31fl3741::kDeghostResistor << 4
                                | syscfg::is31fl3741::kDeghostResistor << 0;
/*
Page 16
Read/Write 
Resets all LEDs to default values.
*/
constexpr uint16_t rReset = 0x3F;
constexpr uint8_t  kReset = 0xAE;
// Other constants
constexpr int kNumRows = 9;
constexpr int kNumCols = 13;


IS31FL3741::IS31FL3741()
  : hI2c_(&I2c::kBus, I2c::kAddr, I2c::kRegSize) {
  act_verifyId();
  act_swReset();
  setup_pullResistor();
  setup_configAndDisable();
  act_refreshBrightness();
}

void IS31FL3741::set_globalCurrent(const uint8_t current) {
  set_page(Page::k4);
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rGlobalCurrent, current);
  if (status != HAL_OK) {Error_Handler();}
}

HAL_StatusTypeDef IS31FL3741::set_pixel(uint32_t row, uint32_t col,
                                         beacon_math::HslColor hsl) {
  if ((row > kNumRows) || (col > kNumCols)) {return HAL_ERROR;}             
  beacon_math::RgbColor rgb = beacon_math::hslToRgb(hsl);
  // return set_pixel(row, col, rgb);
  (this->*set_pixel_func)(row, col, rgb);
}

HAL_StatusTypeDef IS31FL3741::set_row(uint32_t row, beacon_math::HslColor hsl) {
  if (row >= kNumRows) {return HAL_ERROR;}
  beacon_math::RgbColor rgb = beacon_math::hslToRgb(hsl);
  for (int col = 0; col < kNumCols; col++) {
    // this->set_pixel(row, col, rgb);
    (this->*set_pixel_func)(row, col, rgb);
  }
  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_col(uint32_t col, beacon_math::HslColor hsl) {
  if (col >= kNumCols) {return HAL_ERROR;}
  beacon_math::RgbColor rgb = beacon_math::hslToRgb(hsl);
  for (int row = 0; row < kNumRows; row++) {
    // this->set_pixel(row, col, rgb);
    (this->*set_pixel_func)(row, col, rgb);
  }
  return HAL_OK;
}

void IS31FL3741::set_all_pixels(beacon_math::HslColor hsl) {
  beacon_math::RgbColor rgb = beacon_math::hslToRgb(hsl);
  for (int row = 0; row < kNumRows; row++) {
    for (int col = 0; col < kNumCols; col++) {
      // this->set_pixel(row, col, rgb);
      (this->*set_pixel_func)(row, col, rgb);
    }
  }
}

void IS31FL3741::set_pixels_blank() {
  memset(page0_leds_, 0, kPage0ArraySize);
  memset(page1_leds_, 0, kPage1ArraySize);
}

void IS31FL3741::act_verifyId() {
  HAL_StatusTypeDef status;
  uint8_t read_id = 0;
  status = hI2c_.act_pollRead(rDeviceId, &read_id);
  if (status != HAL_OK) {Error_Handler();}
  if (read_id != kDeviceId) {Error_Handler();}
}

void IS31FL3741::act_off() {
  set_page(Page::k4);
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rOperationConfig, kDeviceDisabled);
  if (status != HAL_OK) {Error_Handler();}
}

void IS31FL3741::act_on() {
  set_page(Page::k4);
  HAL_StatusTypeDef status;
  status = hI2c_.act_pollVerifyWrite(rOperationConfig, kDeviceEnabled);
  if (status != HAL_OK) {Error_Handler();}
}

void IS31FL3741::act_refreshBrightness() {
  uint8_t kFullBrightnessPage0[kPage0ArraySize];
  memset(kFullBrightnessPage0, 0xFF, kPage0ArraySize); 
  uint8_t kFullBrightnessPage1[kPage1ArraySize];
  memset(kFullBrightnessPage1, 0xFF, kPage1ArraySize); 
  
  constexpr uint8_t kInitialAddress = 0x00;
  HAL_StatusTypeDef status = HAL_OK;
  set_page(Page::k2);
  status = hI2c_.act_dmaWrite(kInitialAddress, kFullBrightnessPage0, kPage0ArraySize);
  if (status != HAL_OK) {Error_Handler();}
  set_page(Page::k3);
  status = hI2c_.act_dmaWrite(kInitialAddress, kFullBrightnessPage1, kPage1ArraySize);
  if (status != HAL_OK) {Error_Handler();}
}

void IS31FL3741::act_refreshColor() {
  HAL_StatusTypeDef status = HAL_OK;
  constexpr uint16_t kInitialAddress = 0x00;
  set_page(Page::k0);
  status = hI2c_.act_dmaWrite(kInitialAddress, page0_leds_, kPage0ArraySize);
  if (status != HAL_OK) {Error_Handler();}

  set_page(Page::k1);
  status = hI2c_.act_dmaWrite(kInitialAddress, page1_leds_, kPage1ArraySize);                                 
  if (status != HAL_OK) {Error_Handler();}
}

void IS31FL3741::setup_configAndDisable() {
  set_page(Page::k4);
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rOperationConfig, kDeviceDisabled);
  if (status != HAL_OK) {Error_Handler();}
}

void IS31FL3741::setup_pullResistor() {
  set_page(Page::k4);
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rPullResistor, kPullResistor);
  if (status != HAL_OK) {Error_Handler();}
}

void IS31FL3741::set_page(const Page page) {
  if (page == page_) {return;}
  HAL_StatusTypeDef status = hI2c_.act_pollVerifyWrite(rUnlock, kUnlock);
  if (status != HAL_OK) {Error_Handler();}
  status = hI2c_.act_pollWrite(rPageSelect, 
                                static_cast<uint8_t>(page));
  if (status != HAL_OK) {Error_Handler();}
  page_ = page;
}

void IS31FL3741::act_swReset() {
  set_page(Page::k4);
  HAL_StatusTypeDef status = hI2c_.act_pollWrite(rReset, kReset);
  if (status != HAL_OK) {Error_Handler();}
  page_ = Page::k0; // Set to default reset state
  set_pixels_blank();
  status = hI2c_.act_waitForResponse();
  if (status != HAL_OK) {Error_Handler();}
}

HAL_StatusTypeDef IS31FL3741::set_pixel_adafruit5201(uint32_t row, uint32_t col,
                                        beacon_math::RgbColor rgb) {
  if ((row > kNumRows) || (col > kNumCols)) {return HAL_ERROR;}
  // Due to layout, physically remaps the rows
  constexpr uint32_t rowmap[] = {8, 5, 4, 3, 2, 1, 0, 7, 6};
  row =  rowmap[row];

  // Converts 2D (row, col) pixel address cordinates to 1D LED address 
  uint32_t address;
  uint8_t *array;
  if (col >= 10) {
  address = (3 * col) + (9 * row) + 60;
  if (address > kPage1ArraySize) {
  return HAL_ERROR;
  }
  array = page1_leds_;
  }
  else if (row <= 5) {
  address = (3 * col) + (30 * row);
  if (address > kPage0ArraySize) {
  return HAL_ERROR;
  }
  array = page0_leds_;
  }
  else {
  address = (3 * col) + (30 * row) - 180;
  if (address > kPage1ArraySize) {
  return HAL_ERROR;
  }
  array = page1_leds_;
  }

  // RGB colors are flipped on odd columns and column 12
  constexpr uint32_t rOffset = 2;
  constexpr uint32_t gOffset = 1;
  constexpr uint32_t bOffset = 0;
  if ((col & 1) || (col == 12)) {
  constexpr uint32_t remap[] = {2, 0, 1};
  array[address + remap[rOffset]] = rgb.r;
  array[address + remap[gOffset]] = rgb.g;
  array[address + remap[bOffset]] = rgb.b;
  }
  else {
  array[address + rOffset] = rgb.r;
  array[address + gOffset] = rgb.g;
  array[address + bOffset] = rgb.b;
  }
  return HAL_OK;
}
} // namespace is31fl3741

beacon_math::HslColor global_hsl = {0, 255, 0x80};
void Task_Is31fl3741(void *argument) {
  i2c_dma_done_event_flags = osEventFlagsNew(NULL);
  is31fl3741::IS31FL3741 is31fl3741;
  HAL_StatusTypeDef status = HAL_OK;

  is31fl3741.set_globalCurrent(0x02);

  is31fl3741.act_on();

  int delay = 50;
  constexpr int kNumRows = 9;
  constexpr int kNumCols = 13;

  for(;;) {
    // is31fl3741.set_col(0, global_hsl);
    // is31fl3741.act_refreshColor();
    // osDelay(delay);
    // global_hsl.h += 1;

    for (int col = 0; col < kNumCols; col++) {
      is31fl3741.set_pixels_blank();
      is31fl3741.set_col(col, global_hsl);
      is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    }
    for (int row = 0; row < kNumRows; row++) {
      is31fl3741.set_pixels_blank();
      is31fl3741.set_row(row, global_hsl);
      is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    }
    for (int col = (kNumCols - 1); col >= 0; col--)
    {
      is31fl3741.set_pixels_blank();
      is31fl3741.set_col(col, global_hsl);
      is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    }
    for (int row = kNumRows - 1; row >= 0; row--) {
      is31fl3741.set_pixels_blank();
      is31fl3741.set_row(row, global_hsl);
      is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    } 
    global_hsl.h += 5;
  }
} 
