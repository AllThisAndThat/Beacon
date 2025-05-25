#include "ltr_303als.h"

#include "cmsis_os2.h"
#include "main.h"

#include "cpp_main.h"
#include "syscfg.h"

constexpr uint16_t kAddr = syscfg::i2c::addr::kLtr303als;

#define LTR303ALS_FLAG_INT   (1U << 0)
osEventFlagsId_t ltr303als_event_flags;
uint16_t brightness;

Ltr_303als::Ltr_303als() {
  hI2c_ = I2c(syscfg::i2c::bus::kLtr303als);
  last_brightness_ = 0;
  high_threshold_ = 0XFFFF;
  low_threshold_ = 0;
  initiate();
}

Ltr_303als::~Ltr_303als() {

}

HAL_StatusTypeDef Ltr_303als::act_setInterruptThresholds() {
  HAL_StatusTypeDef status;

  status = this->act_pollBrightness();
  if (status != HAL_OK) {return status;}

  // HAL_NVIC_DisableIRQ(EXTI5_IRQn);
  /*
  The following math is used to set an interrupt thereshold that is
  6% larger/smaller than current brightness.
  Uses divider that is power of 2 to avoid floating point math.
  Prevents overflow of the 16 bit number.
  */
 constexpr uint16_t kDenTerm = 32;
  constexpr uint16_t kNumHighTerm = kDenTerm + 1;
  constexpr uint16_t kMaxThreshold = 0xFFFF;
  constexpr uint16_t KMaxValueToOverflow = ((kMaxThreshold / kNumHighTerm) * kDenTerm);
  if (last_brightness_ >= KMaxValueToOverflow) {
    high_threshold_ = kMaxThreshold;
  }
  else {
    high_threshold_ = ((last_brightness_ * kNumHighTerm) / kDenTerm); // kDenTerm);
  }

  constexpr uint16_t rIntThresholdHigh1 = 0x98;
  uint8_t high_threshold_msb = static_cast<uint8_t>((high_threshold_ >> 8) & 0xFF);
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntThresholdHigh1, high_threshold_msb);
  if (status != HAL_OK) {return status;}
  constexpr uint16_t rIntThresholdHigh0 = 0x97;
  uint8_t high_threshold_lsb = static_cast<uint8_t>(high_threshold_ & 0xFF);
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntThresholdHigh0, high_threshold_lsb);
  if (status != HAL_OK) {return status;}

  constexpr uint16_t kNumLowTerm = kDenTerm - 1;
  low_threshold_ = ((last_brightness_ * kNumLowTerm) / kDenTerm);
  constexpr uint16_t rIntThresholdLow1 = 0x9A;
  uint8_t low_threshold_msb = static_cast<uint8_t>((low_threshold_ >> 8) & 0xFF);
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntThresholdLow1, low_threshold_msb);
  if (status != HAL_OK) {return status;}
  low_threshold_ = last_brightness_ >> 1;
  constexpr uint16_t rIntThresholdLow0 = 0x99;
  uint8_t low_threshold_lsb = static_cast<uint8_t>(low_threshold_ & 0xFF);
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntThresholdLow0, low_threshold_lsb);
  if (status != HAL_OK) {return status;}


  if (high_threshold_ < low_threshold_) {
    Error_Handler();
  }

  return status;
}

void Ltr_303als::initiate() {
  HAL_StatusTypeDef status;

  status = this->act_swReset();
  if (status != HAL_OK) {Error_Handler();}

  constexpr uint16_t rMeasIntRate = 0x85;
  constexpr uint8_t kMeasAndIntRate = 0b00'010'010; 
  status = hI2c_.act_pollVerifyWrite(kAddr, rMeasIntRate, kMeasAndIntRate);
  if (status != HAL_OK) {Error_Handler();}

  constexpr uint16_t rIntPersist = 0x9E;
  constexpr uint8_t kIntPersist = 0b0000'0011;
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntPersist, kIntPersist);
  if (status != HAL_OK) {Error_Handler();}

  status = this->act_enableInterrupts();
  if (status != HAL_OK) {Error_Handler();}

  status = this->act_startSampling();
  if (status != HAL_OK) {Error_Handler();}

  status = this->act_setInterruptThresholds();
  if (status != HAL_OK) {Error_Handler();}
}

HAL_StatusTypeDef Ltr_303als::act_disableInterrupts() {
  HAL_StatusTypeDef status = HAL_OK;
  status = this->act_stopSampling();

  constexpr uint16_t rIntConfig = 0x8F;
  constexpr uint8_t kIntConfig = 0b00000'0'0'0; 
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntConfig, kIntConfig);
  return status;
}

HAL_StatusTypeDef Ltr_303als::act_enableInterrupts() {
  HAL_StatusTypeDef status = HAL_OK;
  constexpr uint16_t rIntConfig = 0x8F;
  constexpr uint8_t kIntConfig = 0b00000'1'1'0; // enable active high interrupts
  status = hI2c_.act_pollVerifyWrite(kAddr, rIntConfig, kIntConfig);
  if (status != HAL_OK) {return status;}

  status = this->act_startSampling();
  return status;
}

HAL_StatusTypeDef Ltr_303als::act_pollBrightness() {
  constexpr uint16_t rChan1_LSB = 0X88;
  constexpr uint16_t rChan1_MSB = 0X89;
  constexpr uint16_t rChan0_LSB = 0X8A;
  constexpr uint16_t rChan0_MSB = 0X8B;
 
  /*
  Internal comparisions for thresholds are with channel 0 and not 1.
  Channel 0 and 1 can have vastly different values.
  */

  uint8_t rx_buffer = 0;
  hI2c_.act_pollRead(kAddr, rChan1_LSB, &rx_buffer);
  // data = rx_buffer;
  hI2c_.act_pollRead(kAddr, rChan1_MSB, &rx_buffer);
  // last_brightness_ = data + (rx_buffer << 8);

  hI2c_.act_pollRead(kAddr, rChan0_LSB, &rx_buffer);
  last_brightness_ = rx_buffer;
  hI2c_.act_pollRead(kAddr, rChan0_MSB, &rx_buffer);
  last_brightness_ = last_brightness_ + (rx_buffer << 8);

  return HAL_OK;
}

HAL_StatusTypeDef Ltr_303als::act_stopSampling() {
  HAL_StatusTypeDef status;
  constexpr uint16_t rGainSetting = 0x80;
  constexpr uint8_t kGainSetting = 0b000'111'0'0; // 96X gain, deactivates sensor
  status = hI2c_.act_pollVerifyWrite(kAddr, rGainSetting, kGainSetting);
  if (status != HAL_OK) {Error_Handler();}
  return status;
}

HAL_StatusTypeDef Ltr_303als::act_startSampling() {
  HAL_StatusTypeDef status;
  constexpr uint16_t rGainSetting = 0x80;
  constexpr uint8_t kMaxGainSetting = 0b000'111'0'1; // 96X gain, activates sensor
  status = hI2c_.act_pollVerifyWrite(kAddr, rGainSetting, kMaxGainSetting);
  return status;
}

HAL_StatusTypeDef Ltr_303als::act_swReset() {
  HAL_StatusTypeDef status;
  constexpr uint16_t rSwReset = 0x80;
  constexpr uint8_t  kSwReset = 0b000'000'1'0;
  status = hI2c_.act_pollWrite(kAddr, rSwReset, kSwReset);
  if (status != HAL_OK) {Error_Handler();}
  HAL_Delay(100);
  return status;
}

void Task_Ltr_303als(void *argument) {
  ltr303als_event_flags = osEventFlagsNew(NULL);
  Ltr_303als ltr303als;
  HAL_StatusTypeDef status;
  uint32_t flags;
  for(;;) {
    flags = osEventFlagsWait(ltr303als_event_flags, LTR303ALS_FLAG_INT, osFlagsWaitAny, osWaitForever);
    if (flags & LTR303ALS_FLAG_INT) {
      status = ltr303als.act_setInterruptThresholds();
      if (status != HAL_OK) { Error_Handler();}
      brightness = ltr303als.get_brightness();
    }
    
  }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == AMB_INT_Pin) {
    osEventFlagsSet(ltr303als_event_flags, LTR303ALS_FLAG_INT);
  }
}