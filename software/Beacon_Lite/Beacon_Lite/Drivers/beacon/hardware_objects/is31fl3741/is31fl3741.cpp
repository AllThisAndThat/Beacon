#include "is31fl3741.h"

#include "cmsis_os2.h"

#include "syscfg.h"

/*
TODO:
- update formatting
- add comments
- move registers to g namespace
*/

constexpr int kNumRows = 9;
constexpr int kNumCols = 13;

namespace {
// I2C configuration
  constexpr uint16_t kAddr = syscfg::i2c::addr::kIs31fl3741;
  inline I2C_HandleTypeDef&  kBus  = syscfg::i2c::bus::kIs31fl3741;
  constexpr RegSize  kRegSize = RegSize::k8Bit;

  // Page 14 datasheet
  constexpr uint8_t kSWS_n9        = (0b0000 << 4);
  constexpr uint8_t kLGC_2V4_0V6   = (1 << 3);
  constexpr uint8_t kOSDE_noDetect = (0b00 << 1);
  
  constexpr uint8_t kData = kSWS_n9 | kLGC_2V4_0V6 | kOSDE_noDetect;
  constexpr uint8_t kDataOff = kData | (0 << 0);
  constexpr uint8_t kDataOn  = kData | (1 << 0);
  constexpr uint8_t rRegister = 0x00;

  constexpr uint8_t kUnlock = 0b1100'0101;
  constexpr uint8_t rUnlock = 0xFE;
}

IS31FL3741::IS31FL3741()
  : hI2c_(kBus, kAddr, kRegSize) {
  page_ = Page::k0;
  state_ = IS31FL3741_State::kOff;
  global_current_ = 0x00;

  HAL_StatusTypeDef status = initiate();
  if (status != HAL_OK) {this->error_handler();}
}

IS31FL3741::~IS31FL3741() {

}

HAL_StatusTypeDef IS31FL3741::set_globalCurrent(const uint8_t current) {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  global_current_ = current; //TODO: remove

  constexpr uint16_t rGlobalCurrent = 0x01;
  status = hI2c_.act_pollVerifyWrite(rGlobalCurrent, global_current_);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_pixel(uint32_t row, uint32_t col,
                                         HslColor hsl) {
  if ((row > kNumRows) || (col > kNumCols)) {return HAL_ERROR;}             

  RgbColor rgb = this->act_hslToRgb(hsl);
  
  return set_pixel(row, col, rgb);
}

HAL_StatusTypeDef IS31FL3741::set_pixel(uint32_t row, uint32_t col,
                                        RgbColor rgb) {
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

HAL_StatusTypeDef IS31FL3741::set_row(uint32_t row, HslColor hsl) {
  if (row >= kNumRows) {return HAL_ERROR;}

  RgbColor rgb = this->act_hslToRgb(hsl);

  for (int col = 0; col < kNumCols; col++) {
    this->set_pixel(row, col, rgb);
  }

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_col(uint32_t col, HslColor hsl) {
  if (col >= kNumCols) {return HAL_ERROR;}

  RgbColor rgb = this->act_hslToRgb(hsl);

  for (int row = 0; row < kNumRows; row++) {
    this->set_pixel(row, col, rgb);
  }

  return HAL_OK;
}

void IS31FL3741::set_all_pixels(HslColor hsl) {
  RgbColor rgb = this->act_hslToRgb(hsl);

  for (int row = 0; row < kNumRows; row++) {
    for (int col = 0; col < kNumCols; col++) {
      this->set_pixel(row, col, rgb);
    }
  }
}

void IS31FL3741::set_all_pixels_blank() {
  for (int i = (kPage0ArraySize - 1); i >= 0; i--) {
    page0_leds_[i] = 0;
  }
  for (int i = (kPage1ArraySize - 1); i >= 0; i--) {
    page1_leds_[i] = 0;
  }
}

HAL_StatusTypeDef IS31FL3741::act_verify() {
  HAL_StatusTypeDef status;

  constexpr uint8_t rId = 0xFC;
  uint8_t readId = 0;
  status = hI2c_.act_pollRead(rId, &readId);
  constexpr uint8_t kCorrectId = 0x60;
  if (readId != kCorrectId) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_off() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  status = hI2c_.act_pollVerifyWrite(rRegister, kDataOff);
  if (status != HAL_OK) {this->error_handler();}

  state_ = IS31FL3741_State::kOff;

  return status;
}

HAL_StatusTypeDef IS31FL3741::act_on() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  status = hI2c_.act_pollVerifyWrite(rRegister, kDataOn);
  if (status != HAL_OK) {this->error_handler();}

  state_ = IS31FL3741_State::kOn;

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_resetAllLeds() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  constexpr uint8_t rReset = 0x3F;
  constexpr uint8_t kReset = 0xAE;
  status = hI2c_.act_pollWrite(rReset, kReset);
  if (status != HAL_OK) {this->error_handler();}
  page_ = Page::k0; // Default value after reset

  // Restore non-led registers
  status = setup_configRegister();
  if (status != HAL_OK) {this->error_handler();}
  status = setup_pullResistor();
  if (status != HAL_OK) {this->error_handler();}
  status = set_globalCurrent(global_current_);
  if (status != HAL_OK) {this->error_handler();}

  if (state_ == IS31FL3741_State::kOn) {
    status = act_on();
    if (status != HAL_OK) {this->error_handler();}
  }

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_refreshBrightness() {
  HAL_StatusTypeDef status;

  status = set_page(Page::k2);
  if (status != HAL_OK) {this->error_handler();}
  for (int i = 0; i < kPage0ArraySize; i++) {
    status = hI2c_.act_pollVerifyWrite(i, 0xFF);
    if (status != HAL_OK) {this->error_handler();}
  }

  status = set_page(Page::k3);
  for (int i = 0; i < kPage1ArraySize; i++) {
    if (status != HAL_OK) {this->error_handler();}
    status = hI2c_.act_pollVerifyWrite(i, 0xFF);
    if (status != HAL_OK) {this->error_handler();}
  }

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_refreshColor() {
  HAL_StatusTypeDef status;

  status = set_page(Page::k0);
  if (status != HAL_OK) {this->error_handler();}
  status = HAL_I2C_Mem_Write_DMA(&hi2c1, kAddr, 0, I2C_MEMADD_SIZE_8BIT,
                                 page0_leds_, kPage0ArraySize);
  if (status != HAL_OK) {this->error_handler();}

  status = set_page(Page::k1);
  if (status != HAL_OK) {this->error_handler();}
  status = HAL_I2C_Mem_Write_DMA(&hi2c1, kAddr, 0, I2C_MEMADD_SIZE_8BIT,
                                 page1_leds_, kPage1ArraySize);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::initiate() {
  HAL_StatusTypeDef status;

  status = act_verify();
  if (status != HAL_OK) {this->error_handler();}

  status = act_resetAllLeds();
  if (status != HAL_OK) {this->error_handler();}

  status = setup_configRegister();
  if (status != HAL_OK) {this->error_handler();}

  status = setup_pullResistor();
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::setup_configRegister() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  status = hI2c_.act_pollVerifyWrite(rRegister, kDataOff);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::setup_pullResistor() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  // Datasheet Page 15
  // Saw ghosting with 16k
  constexpr uint8_t kPDR_8k = (0b101 << 4); 
  constexpr uint8_t kPUR_8k = (0b101 << 0);
  constexpr uint8_t kPullResistor = kPDR_8k | kPUR_8k;
  constexpr uint8_t rPullResistor = 0x02;
  status = hI2c_.act_pollVerifyWrite(rPullResistor, kPullResistor);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_page(const Page page) {
  if (page == page_) {return HAL_OK;}

  HAL_StatusTypeDef status = hI2c_.act_pollVerifyWrite(rUnlock, kUnlock);
  if (status != HAL_OK) {this->error_handler();}

  constexpr uint8_t rPageSelect = 0xFD;
  status = hI2c_.act_pollWrite(rPageSelect, 
                                static_cast<uint8_t>(page));
  if (status != HAL_OK) {this->error_handler();}

  page_ = page;
  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_writePage(const Page page,
                                        uint8_t* data,
                                        const size_t data_len) {
  HAL_StatusTypeDef status = set_page(page);
  if (status != HAL_OK) {this->error_handler();}

  constexpr uint8_t rInitialAddress = 0x00;
  status = hI2c_.act_pollWrite(rInitialAddress, data, data_len);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

RgbColor IS31FL3741::act_hslToRgb(HslColor hsl) {
  RgbColor rgb;
  if (hsl.l == 0) {
    return rgb ={0, 0, 0};
  }

  uint16_t l1 = hsl.l + 1;
  uint8_t c;
  if (hsl.l < 128) {
    c = ((l1 << 1) * hsl.s) >> 8;
  }
  else {
    c = (512 - (l1 << 1)) * hsl.s >> 8;
  }

  uint16_t H = hsl.h * 6;              // 0 to 1535 (actually 1530)
  uint8_t lo = H & 255;           // Low byte  = primary/secondary color mix
  uint16_t h1 = lo + 1;
  uint8_t x;

  if ((H & 256) == 0) {
    x = h1 * c >> 8;   // even sextant, like red to yellow
  }        
  else {
    x = (256 - h1) * c >> 8;  // odd sextant, like yellow to green
  }

  uint8_t m = hsl.l - (c >> 1);
  uint8_t  r, g, b ;
  switch(H >> 8) {       // High byte = sextant of colorwheel
  case 0 : r = c; g = x; b = 0; break; // R to Y
  case 1 : r = x; g = c; b = 0; break; // Y to G
  case 2 : r = 0; g = c; b = x; break; // G to C
  case 3 : r = 0; g = x; b = c; break; // C to B
  case 4 : r = x; g = 0; b = c; break; // B to M
  default: r = c; g = 0; b = x; break; // M to R
  }

  rgb.r = r + m;
  rgb.g = g + m;
  rgb.b = b + m;
  return rgb;
}


HslColor global_hsl = {0, 0, 30};
void Task_Is31fl3741(void *argument) {
  IS31FL3741 is31fl3741;
  HAL_StatusTypeDef status;

  status = is31fl3741.set_globalCurrent(0x01);
  if (status != HAL_OK) { Error_Handler();}

  status = is31fl3741.act_on();
  if (status != HAL_OK) { Error_Handler();}

  status = is31fl3741.act_refreshBrightness();
  
  int delay = 40;

  for(;;) {
    for (int col = 0; col < kNumCols; col++) {
      is31fl3741.set_all_pixels_blank();
      is31fl3741.set_col(col, global_hsl);
      status = is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    }
    for (int row = 0; row < kNumRows; row++) {
      is31fl3741.set_all_pixels_blank();
      is31fl3741.set_row(row, global_hsl);
      status = is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    }
    for (int col = (kNumCols - 1); col >= 0; col--)
    {
      is31fl3741.set_all_pixels_blank();
      is31fl3741.set_col(col, global_hsl);
      status = is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    }
    for (int row = kNumRows - 1; row >= 0; row--) {
      is31fl3741.set_all_pixels_blank();
      is31fl3741.set_row(row, global_hsl);
      status = is31fl3741.act_refreshColor();
      global_hsl.s += 1;
      osDelay(delay);
    } 
    global_hsl.h += 5;
  }
}

void IS31FL3741::error_handler() {
  while (1) {
      
  }
}