#pragma once

#include "stm32h5xx_hal.h"

#include "i2c_device.h"

#ifdef __cplusplus
namespace ltr303als {

extern "C" {
void Task_Ltr_303als(void *argument);
}

class Ltr_303als {
 public:
  Ltr_303als();
  void init();

  uint16_t get_brightness() const { return last_brightness_;};
  bool get_isInit() const {return isInit_;};

  friend void Task_Ltr_303als(void *argument);

 private:
  bool isInit_ = false;
  uint16_t last_brightness_;
  uint16_t high_threshold_; 
  uint16_t low_threshold_;
  I2cDevice hI2c_;

  void setup_measureConfig();
  void setup_interruptConfig();

  void act_disableInterrupts();
  void act_enableInterrupts();
  HAL_StatusTypeDef act_pollBrightness();
  HAL_StatusTypeDef act_setInterruptThresholds();
  void act_startSampling();
  void act_stopSampling();
  void act_swReset();
  void act_verifyPartId();
};
} // namespace ltr303als
#endif // extern "C"
