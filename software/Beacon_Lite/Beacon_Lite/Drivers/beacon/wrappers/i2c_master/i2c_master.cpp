#include "i2c_master.h"

#include "main.h"

I2c::I2c(I2C_HandleTypeDef hI2c) {
  this->hI2c_ = hI2c;
}

I2c::~I2c() {
}

HAL_StatusTypeDef I2c::act_pollRead(const uint16_t device_address, const uint16_t register_address,
                                    uint8_t *data) {
  HAL_StatusTypeDef status;
  constexpr uint16_t kDataSize = sizeof(uint8_t);
  status = HAL_I2C_Mem_Read(&this->hI2c_, device_address,
                            register_address, I2C_MEMADD_SIZE_8BIT,
                            data, kDataSize, 10);
  return status;
}

HAL_StatusTypeDef I2c::act_pollWrite(const uint16_t device_address, const uint16_t register_address, uint8_t data) {
  HAL_StatusTypeDef status;
  constexpr uint16_t kDataSize = sizeof(uint8_t);
  status = HAL_I2C_Mem_Write(&this->hI2c_, device_address,
                             register_address, I2C_MEMADD_SIZE_8BIT,
                             &data, kDataSize, 10);
  return status;
}

HAL_StatusTypeDef I2c::act_pollVerifyWrite(const uint16_t device_address, const uint16_t register_address, uint8_t data) {
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t rx_buffer;
  this->act_pollWrite(device_address, register_address, data);
  this->act_pollRead(device_address, register_address, &rx_buffer);
  if (rx_buffer != data) {status = HAL_ERROR;}
  return status;
}