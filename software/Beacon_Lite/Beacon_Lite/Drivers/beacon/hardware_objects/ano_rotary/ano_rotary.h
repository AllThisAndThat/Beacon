/*
https://github.com/adafruit/Adafruit_Seesaw
- adafruit_seesaw
/examples/encoder/PID5740_ANOencoder_demo
https://www.adafruit.com/product/6310

TODO: 
Fix up I2C
  - Convert I2C_Master to I2C_Device that has paramenter device address
  - Methods: 
    - act_pollRead
    - act_pollRead
    - act_dmaRead
    - act_pollWrite
    - act_pollWrite
    - act_dmaWrite

    - use set and get functions not act_get, act_set
*/
#pragma once

#include "stm32h5xx_hal.h"

#include "i2c_device.h"

#ifdef __cplusplus
namespace ano_rotary {
enum Button : uint8_t {
  kSelect = 1 << 1,
  kUp     = 1 << 2,
  kLeft   = 1 << 3,
  kDown   = 1 << 4,
  kRight  = 1 << 5,
  kAll    = 0b00111110
};

extern "C" {
void Task_AnoEncoder(void *argument);
}

class AnoRotary {
 public:
  AnoRotary();
  void init();

  uint8_t get_buttonState(Button button) const;
  uint32_t get_encoderPosition() const {
    return encoderPosition_;
  };
  bool get_isInit() const {return isInit_;};

  void act_waitForButtonRelease(Button button);

  friend void Task_AnoEncoder(void *argument);

 private:
  bool isInit_ = false;
  I2cDevice hI2c_;
  uint8_t buttonState_ = 0;
  uint32_t encoderPosition_ = 0;

  void act_enableEncoderInts(); // TODO: might want ability to disable interrupts on encoder
  void act_enableButtonInts();
  void act_readButtonState();
  void act_readEncoderPosition();
  void act_setButtonInputPU();
  void act_SWReset();
  HAL_StatusTypeDef act_verify();
};
} // namespace ano_rotary
#endif // extern "C"