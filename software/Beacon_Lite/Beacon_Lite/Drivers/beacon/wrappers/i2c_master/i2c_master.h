#pragma once

#include "stm32h5xx_hal.h"

class I2c {
public:
  I2c() = default;
  I2c(I2C_HandleTypeDef hI2c);
  ~I2c();

  HAL_StatusTypeDef act_pollRead(const uint16_t device_address, const uint16_t register_address,
                                 uint8_t *data);
  HAL_StatusTypeDef act_pollWrite(const uint16_t device_address, const uint16_t register_address, uint8_t data);
  HAL_StatusTypeDef act_pollVerifyWrite(const uint16_t device_address, const uint16_t register_address, uint8_t data);

private:
  I2C_HandleTypeDef hI2c_;
};