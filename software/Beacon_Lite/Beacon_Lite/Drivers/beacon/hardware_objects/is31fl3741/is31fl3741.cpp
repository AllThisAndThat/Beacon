#include "is31fl3741.h"

#include "syscfg.h"

constexpr uint8_t kAddr = syscfg::i2c::addr::kIs31fl3741;

namespace IS31FL3741_Config {
  // Page 14 datasheet
  constexpr uint8_t kSWS_n9        = (0b0000 << 4);
  constexpr uint8_t kLGC_2V4_0V6   = (1 << 3);
  constexpr uint8_t kOSDE_noDetect = (0b00 << 1);
  
  constexpr uint8_t kData = kSWS_n9 | kLGC_2V4_0V6 | kOSDE_noDetect;
  constexpr uint8_t kDataOff = kData | (0 << 0);
  constexpr uint8_t kDataOn  = kData | (1 << 0);
  constexpr uint8_t rRegister = 0x00;
}

IS31FL3741::IS31FL3741()
  : hI2c_(syscfg::i2c::bus::kIs31fl3741) {
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

  global_current_ = current;

  constexpr uint8_t rGlobalCurrent = 0x01;
  status = hI2c_.act_pollVerifyWrite(kAddr, rGlobalCurrent, current);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_specificLed(uint32_t led_num,
                                       const uint8_t pwm,
                                       const uint8_t scale) {
  constexpr uint32_t kMaxLedNum = 351; 
  if (led_num >= kMaxLedNum) {return HAL_ERROR;}

  HAL_StatusTypeDef status;
  constexpr uint32_t kGroup1Max = 180;
  if (led_num < kGroup1Max) {
    status = set_page(Page::k0);
    if (status != HAL_OK) {this->error_handler();}
    status = hI2c_.act_pollVerifyWrite(kAddr, led_num, pwm);
    if (status != HAL_OK) {this->error_handler();}

    status = set_page(Page::k2);
    if (status != HAL_OK) {this->error_handler();}
    status = hI2c_.act_pollVerifyWrite(kAddr, led_num, scale);
    if (status != HAL_OK) {this->error_handler();}
  } 
  else {
    led_num -= kGroup1Max;
    status = set_page(Page::k1);
    if (status != HAL_OK) {this->error_handler();}
    status = hI2c_.act_pollVerifyWrite(kAddr, led_num, pwm);
    if (status != HAL_OK) {this->error_handler();}

    status = set_page(Page::k3);
    if (status != HAL_OK) {this->error_handler();}
    status = hI2c_.act_pollVerifyWrite(kAddr, led_num, scale);
    if (status != HAL_OK) {this->error_handler();}
  }
  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_pixel(LedConfig config) {
  constexpr uint32_t kMaxX = 12;
  constexpr uint32_t kMaxY = 8;
  if ((config.x > kMaxX) || (config.y > kMaxY)) {
    return HAL_ERROR;
  }

  // Maps {0, 1, 2, 3, 4, 5, 6, 7, 8} to {8, 5, 4, 3, 2, 1, 0, 7, 6}
  constexpr uint32_t rowmap[] = {8, 5, 4, 3, 2, 1, 0, 7, 6};
  config.y = rowmap[config.y];

  uint32_t offset;
  if (config.x < 10) {
    offset = ((config.y * 10) + config.x) * 3;
  }
  else {
    offset = (((config.y * 3) + 80) + config.x) * 3;
  }

  constexpr uint32_t rOffset = 2;
  constexpr uint32_t gOffset = 1;
  constexpr uint32_t bOffset = 0;

  // Color correction. Specific to setup. 3.3V in with Adafruit led board
  constexpr uint8_t kRedFactor = 20; 
  constexpr uint8_t kGreenFactor = 100; 
  constexpr uint8_t kBlueFactor = 255;
  if ((config.x & 1) || (config.x == 12)) {
       constexpr uint32_t remap[] = {2, 0, 1};
    set_specificLed((offset + remap[rOffset]), config.r, kRedFactor);
    set_specificLed((offset + remap[gOffset]), config.g, kGreenFactor);
    set_specificLed((offset + remap[bOffset]), config.b, kBlueFactor);
  }
  else {
    set_specificLed((offset + rOffset), config.r, kRedFactor);
    set_specificLed((offset + gOffset), config.g, kGreenFactor);
    set_specificLed((offset + bOffset), config.b, kBlueFactor);
  }

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_verify() {
  HAL_StatusTypeDef status;

  constexpr uint8_t kCorrectId = 0x60;
  constexpr uint8_t rId = 0xFC;
  uint8_t readId = 0;
  status = hI2c_.act_pollRead(kAddr, rId, &readId);
  if (readId != kCorrectId) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_off() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  using namespace IS31FL3741_Config;
  status = hI2c_.act_pollVerifyWrite(kAddr, rRegister, kDataOff);
  if (status != HAL_OK) {this->error_handler();}

  state_ = IS31FL3741_State::kOff;

  return status;
}

HAL_StatusTypeDef IS31FL3741::act_on() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  using namespace IS31FL3741_Config;
  status = hI2c_.act_pollVerifyWrite(kAddr, rRegister, kDataOn);
  if (status != HAL_OK) {this->error_handler();}

  state_ = IS31FL3741_State::kOn;

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_resetAllLeds() {
  HAL_StatusTypeDef status = set_page(Page::k4);
  if (status != HAL_OK) {this->error_handler();}

  constexpr uint8_t rReset = 0x3F;
  constexpr uint8_t kReset = 0xAE;
  status = hI2c_.act_pollWrite(kAddr, rReset, kReset);
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

  using namespace IS31FL3741_Config;
  status = hI2c_.act_pollVerifyWrite(kAddr, rRegister, kDataOff);
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
  status = hI2c_.act_pollVerifyWrite(kAddr, rPullResistor,
                                kPullResistor);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::set_page(const Page page) {
  if (page == page_) {return HAL_OK;}
  page_ = page;

  constexpr uint8_t kUnlock = 0b1100'0101;
  constexpr uint8_t rUnlock = 0xFE;
  HAL_StatusTypeDef status = hI2c_.act_pollVerifyWrite(kAddr, rUnlock, kUnlock);
  if (status != HAL_OK) {this->error_handler();}

  constexpr uint8_t rPageSelect = 0xFD;
  status = hI2c_.act_pollWrite(kAddr, rPageSelect, 
                                static_cast<uint8_t>(page));
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

HAL_StatusTypeDef IS31FL3741::act_writePage(const Page page,
                                        uint8_t* data,
                                        const size_t data_len) {
  HAL_StatusTypeDef status = set_page(page);
  if (status != HAL_OK) {this->error_handler();}

  constexpr uint8_t rInitialAddress = 0x00;
  status = hI2c_.act_pollWriteMulti(kAddr, rInitialAddress, data,
                                  data_len);
  if (status != HAL_OK) {this->error_handler();}

  return HAL_OK;
}

void IS31FL3741::error_handler() {
  while (1) {
      
  }
}