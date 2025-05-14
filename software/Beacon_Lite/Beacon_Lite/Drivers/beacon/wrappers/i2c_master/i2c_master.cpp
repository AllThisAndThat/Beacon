#include "i2c_master.h"

#include "main.h"

I2c::I2c(I2C_HandleTypeDef hI2c, uint16_t device_address) {
  this->hI2c_ = hI2c;
  this->device_address_ = device_address;

  initiate();
}

I2c::~I2c() {
}

HAL_StatusTypeDef I2c::initiate() {
  hI2c_.Instance = I2C1;
  hI2c_.Init.Timing = 0x00300F38;
  hI2c_.Init.OwnAddress1 = 0;
  hI2c_.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hI2c_.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hI2c_.Init.OwnAddress2 = 0;
  hI2c_.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c_.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hI2c_.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hI2c_) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hI2c_, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hI2c_, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
  return HAL_OK;
}

HAL_StatusTypeDef I2c::act_pollRead(const uint16_t register_address,
                                    uint8_t *data) {
  HAL_StatusTypeDef status;
  constexpr uint16_t kDataSize = sizeof(uint8_t);
  status = HAL_I2C_Mem_Read(&this->hI2c_, this->device_address_,
                            register_address, I2C_MEMADD_SIZE_8BIT,
                            data, kDataSize, 10);
  return status;
}

HAL_StatusTypeDef I2c::act_pollWrite(const uint16_t register_address, uint8_t data) {
  HAL_StatusTypeDef status;
  constexpr uint16_t kDataSize = sizeof(uint8_t);
  status = HAL_I2C_Mem_Write(&this->hI2c_, this->device_address_,
                             register_address, I2C_MEMADD_SIZE_8BIT,
                             &data, kDataSize, 10);
  return status;
}