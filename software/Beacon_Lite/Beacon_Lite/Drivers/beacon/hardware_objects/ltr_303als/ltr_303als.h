#include "stm32h5xx_hal.h"

#include "i2c_master.h"

class Ltr_303als {
public:
  Ltr_303als(I2C_HandleTypeDef hI2c);
  ~Ltr_303als();
  HAL_StatusTypeDef initiate();

  HAL_StatusTypeDef act_pollBrightness(uint32_t *brightness);


private:
  I2c hI2c_;
};