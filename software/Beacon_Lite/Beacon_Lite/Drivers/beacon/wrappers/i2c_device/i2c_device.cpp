#include "i2c_device.h"

#include "main.h"
#include <cstring>

#include "app_freertos.h"
/*
TODO: 
Fix up I2C
- fix LTR-303 ALS
- fix IS31FL3741
- fix ano_rotary

*/
namespace {
  constexpr uint32_t kTimeout = 10; // In milliseconds
}


I2cDevice::I2cDevice(I2C_HandleTypeDef hI2c, const uint16_t device_addr, 
                     const RegSize reg_size) {
  this->i2c_bus_ = hI2c;
  this->dev_addr_ = device_addr;
  this->reg_size_ = reg_size;
}

I2cDevice::~I2cDevice() {

}

// TODO: something is wrong here
HAL_StatusTypeDef I2cDevice::act_pollRead(const uint16_t reg_addr,
                                    uint8_t *data, const size_t size) {
  HAL_StatusTypeDef status;
  osMutexAcquire(i2c_mutexHandle, osWaitForever);
  status = HAL_I2C_Mem_Read(&i2c_bus_, dev_addr_, reg_addr,
                            reg_size_, data, size,
                            kTimeout);
  osMutexRelease(i2c_mutexHandle);
  return status;
}

HAL_StatusTypeDef I2cDevice::act_dmaRead(const uint16_t reg_addr,
  uint8_t *data, const size_t size) {
// DMA read is not implemented in this example
  return HAL_ERROR;
}

HAL_StatusTypeDef I2cDevice::act_pollWrite(const uint16_t reg_addr,
                                           uint8_t data) {
  HAL_StatusTypeDef status;
  osMutexAcquire(i2c_mutexHandle, osWaitForever);
  status = HAL_I2C_Mem_Write(&i2c_bus_, dev_addr_,
                             reg_addr, reg_size_,
                             &data, 1, kTimeout);
  osMutexRelease(i2c_mutexHandle);
  return status;
}

HAL_StatusTypeDef I2cDevice::act_pollWrite(const uint16_t reg_addr,
                                           uint8_t *data,
                                           const size_t size) {
  HAL_StatusTypeDef status;
  osMutexAcquire(i2c_mutexHandle, osWaitForever);
  status = HAL_I2C_Mem_Write(&i2c_bus_, dev_addr_,
                             reg_addr, reg_size_,
                             data, size, kTimeout);
  osMutexRelease(i2c_mutexHandle);
  return status;
}

HAL_StatusTypeDef I2cDevice::act_pollVerifyWrite(const uint16_t reg_addr,
                                                 uint8_t data) {
  HAL_StatusTypeDef status;
  status = this->act_pollWrite(reg_addr, data);
  if (status != HAL_OK) { return status;}

  uint8_t rx_buffer;
  this->act_pollRead(reg_addr, &rx_buffer);

  if (rx_buffer == data) {
    return HAL_OK;
  }
  else {
    return HAL_ERROR;
  }
}

HAL_StatusTypeDef I2cDevice::act_pollVerifyWrite(const uint16_t reg_addr,
                                                 uint8_t *data,
                                                 const size_t size) {
  HAL_StatusTypeDef status;
  status = this->act_pollWrite(reg_addr, data, size);
  if (status != HAL_OK) { return status;}

  constexpr int kMaxSize = 256;
  uint8_t rx_buffer[kMaxSize] = {0};
  status = this->act_pollRead(reg_addr, rx_buffer, size);
  if (status != HAL_OK) { return status;}

  // Compare the data written with the data read back
  if (memcmp(data, rx_buffer, size) == 0) {
    return HAL_OK;
  }
  else {
    return HAL_ERROR;
  }
}

HAL_StatusTypeDef I2cDevice::act_dmaWrite(const uint16_t reg_addr,
                                           const uint8_t *data,
                                           const size_t size) {
  // DMA write is not implemented in this example
  return HAL_ERROR;
}