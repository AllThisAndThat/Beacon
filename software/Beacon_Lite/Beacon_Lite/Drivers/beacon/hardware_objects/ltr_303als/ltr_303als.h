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

  HAL_StatusTypeDef act_setInterruptThresholds();

private:
  I2cDevice hI2c_;
  uint16_t last_brightness_;
  uint16_t high_threshold_;
  uint16_t low_threshold_;

  void initiate();
  HAL_StatusTypeDef act_disableInterrupts();
  HAL_StatusTypeDef act_enableInterrupts();
  HAL_StatusTypeDef act_pollBrightness();
  HAL_StatusTypeDef act_stopSampling();
  HAL_StatusTypeDef act_startSampling();
  HAL_StatusTypeDef act_swReset();
};