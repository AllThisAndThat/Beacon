#pragma once

#include "stm32h5xx_hal.h"

#include "i2c_device.h"

#ifdef __cplusplus
extern "C" {
#endif

void Task_Ltr_303als(void *argument);

#ifdef __cplusplus
}
#endif

class Ltr_303als {
 public:
  Ltr_303als();
  ~Ltr_303als();

  uint16_t get_brightness() const { return last_brightness_;};

  friend void Task_Ltr_303als(void *argument);

 private:
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