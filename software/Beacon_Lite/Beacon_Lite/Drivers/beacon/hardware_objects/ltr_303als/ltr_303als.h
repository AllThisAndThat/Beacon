#include "stm32h5xx_hal.h"

#include "i2c_master.h"

class Ltr_303als {
public:
  Ltr_303als(I2C_HandleTypeDef hI2c);
  ~Ltr_303als();

  HAL_StatusTypeDef get_brightness(uint16_t *brightness);

  HAL_StatusTypeDef act_setInterruptThresholds();

private:
  I2c hI2c_;
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