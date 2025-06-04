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


enum class Button : uint8_t {
  kSelect = 1 << 1,
  kUp     = 1 << 2,
  kLeft   = 1 << 3,
  kDown   = 1 << 4,
  kRight  = 1 << 5,
  kAll    = 0b00111110
};

// enum class ButtonState {
//   kNotPresed = 0,
//   kPressed = 1
// };

#ifdef __cplusplus
extern "C" {
#endif

void Task_AnoEncoder(void *argument);

#ifdef __cplusplus
}
#endif

class AnoRotary {
public:
  AnoRotary();
  ~AnoRotary();

  uint8_t get_buttonState() const {
    return buttonState_;
  };
  uint32_t get_encoderPosition() const {
    return encoderPosition_;
  };

  friend void Task_AnoEncoder(void *argument);
private:
  I2cDevice hI2c_;
  uint8_t buttonState_ = 0;
  uint32_t encoderPosition_ = 0;

  void initiate();

  void act_enableEncoderInts(); // TODO: might want ability to disable interrupts on encoder
  void act_enableButtonInts();
  void act_readButtonState();
  void act_readEncoderPosition();
  void act_setButtonInputPU();
  void act_SWReset();
  HAL_StatusTypeDef act_verify();
};