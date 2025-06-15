#pragma once

#include "stm32h5xx_hal.h"

#ifdef __cplusplus
enum RegSize
{
  k8Bit = I2C_MEMADD_SIZE_8BIT,
  k16Bit = I2C_MEMADD_SIZE_16BIT
};


class I2cDevice {
public:
  I2cDevice() = default;
  I2cDevice(I2C_HandleTypeDef* bus, const uint16_t dev_addr,
            const RegSize reg_size);
  // : i2c_bus_(bus), dev_addr_(dev_addr), reg_size_(reg_size) {};
  ~I2cDevice();

  HAL_StatusTypeDef act_pingDevice();
  HAL_StatusTypeDef act_waitForResponse();

  HAL_StatusTypeDef act_pollRead(const uint16_t reg_addr,
                                 uint8_t *data, 
                                 const size_t size = 1);                               
  HAL_StatusTypeDef act_dmaRead(const uint16_t reg_addr,
                                uint8_t *data, 
                                const size_t size = 1);
  
  HAL_StatusTypeDef act_pollWrite(const uint16_t reg_addr, 
                                  uint8_t data);                              
  HAL_StatusTypeDef act_pollWrite(const uint16_t reg_addr, 
                                  uint8_t *data, 
                                  const size_t size = 1);
  HAL_StatusTypeDef act_pollVerifyWrite(const uint16_t reg_addr,
                                        uint8_t data);                                    
  HAL_StatusTypeDef act_pollVerifyWrite(const uint16_t reg_addr,
                                        uint8_t *data,
                                        const size_t size = 1);
  HAL_StatusTypeDef act_dmaWrite(const uint16_t reg_addr,
                                 uint8_t *data, 
                                 const size_t size = 1);
private:
  I2C_HandleTypeDef* i2c_bus_;
  uint16_t dev_addr_;
  RegSize reg_size_;
};
#endif // extern "C"