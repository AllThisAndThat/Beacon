#include "ltr_303als.h"

#include "main.h"

#include "syscfg.h"

constexpr uint16_t kAddr = syscfg::i2c1::addr::kLtr303als;

Ltr_303als::Ltr_303als(I2C_HandleTypeDef hI2c) {
  hI2c_ = I2c(hI2c);
  initiate();
}

Ltr_303als::~Ltr_303als() {

}

HAL_StatusTypeDef Ltr_303als::initiate() {
  HAL_StatusTypeDef status;

  constexpr uint16_t rGainSetting = 0x80;
  constexpr uint8_t  kMaxGainSetting = 0b000'111'0'1; // 96X gain, activates sensor
  status = hI2c_.act_pollWrite(kAddr, rGainSetting, kMaxGainSetting);
  if (status != HAL_OK) {Error_Handler();}
  uint8_t rx_buffer = 0x0;
  hI2c_.act_pollRead(kAddr, rGainSetting, &rx_buffer);
  if (rx_buffer != kMaxGainSetting) {
    Error_Handler();
  }
  
  constexpr uint8_t rMeasIntRate = 0x85;
  constexpr uint8_t kMeasAndIntRate = 0b00'000'011; // 100 ms integration time, 500 ms measurement rate
  status = hI2c_.act_pollWrite(kAddr, rMeasIntRate, kMeasAndIntRate);
  hI2c_.act_pollRead(kAddr, rMeasIntRate, &rx_buffer);
  if (rx_buffer != kMeasAndIntRate) {
    Error_Handler();
  }

  return HAL_OK;
}

HAL_StatusTypeDef Ltr_303als::act_pollBrightness(uint32_t *brightness) {
  constexpr uint8_t rChan1_LSB = 0X88;
  constexpr uint8_t rChan1_MSB = 0X89;
  constexpr uint8_t rChan0_LSB = 0X8A;
  constexpr uint8_t rChan0_MSB = 0X8B;
  uint32_t data;
  uint8_t rx_buffer = 0;
  hI2c_.act_pollRead(kAddr, rChan1_LSB, &rx_buffer);
  data = rx_buffer;
  hI2c_.act_pollRead(kAddr, rChan1_MSB, &rx_buffer);
  data = data + (rx_buffer << 8);

  hI2c_.act_pollRead(kAddr, rChan0_LSB, &rx_buffer);
  data = data + rx_buffer;
  hI2c_.act_pollRead(kAddr, rChan0_MSB, &rx_buffer);
  *brightness = data + (rx_buffer << 8);

  return HAL_OK;
}