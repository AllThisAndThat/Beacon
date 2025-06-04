/*
https://www.bing.com/search?qs=HS&pq=ltr&sk=CSYN1UAS7AS6LS7MT1&sc=25-3&pglt=161&q=ltr303als+datasheet&cvid=68b580a5fbcc4c0993479d1f6cbaba49&gs_lcrp=EgRlZGdlKgcIABAAGPkHMgcIABAAGPkHMgYIARBFGDkyBggCEAAYQDIGCAMQABhAMgYIBBAAGEAyBggFEAAYQDIGCAYQRRg8MgYIBxBFGDwyBggIEEUYPNIBCDQyODlqMGoxqAIAsAIA&FORM=ANNTA1&PC=NMTS

This IC doesn't support sequential writes and reads.
*/

#include "ltr_303als.h"

#include "cmsis_os2.h"
#include "main.h"

#include "cpp_main.h"
#include "syscfg.h"

uint16_t brightness;

namespace {
// I2C configuration
constexpr uint16_t  kAddr = syscfg::i2c::addr::kLtr303als;
inline I2C_HandleTypeDef&   kBus = syscfg::i2c::bus::kLtr303als;
constexpr RegSize   kRegSize = RegSize::k8Bit;

// Registers
// Page 15
constexpr uint16_t rControlSettings = 0x80;
constexpr uint8_t kGainSettings = 0b000'011'00; // 8X gain
constexpr uint8_t kGainAndDeactive = kGainSettings + 0b0000000'0;
constexpr uint8_t kMaxGainAndActive = kGainSettings + 0b0000000'1;
constexpr uint8_t kSwReset = 0b000'000'1'0;

// Page 16
constexpr uint16_t rMeasIntRate = 0x85;
constexpr uint8_t kIntegrationTime = 0b00'010'000; // 200 ms
constexpr uint8_t kMeasRate = 0b00'000'010; // 200 ms
constexpr uint8_t kMeasAndIntRate = kIntegrationTime + kMeasRate; 

// Page 17
constexpr uint16_t rPartIdAndRev = 0x86;
constexpr uint8_t kPartId = 0b1010'0000;
constexpr uint8_t kPartRev = 0b0000'0000; // Revision 0

// Page 18
constexpr uint16_t rChan1LSB = 0X88;

constexpr uint16_t rChan1MSB = 0X89;

// Page 19
constexpr uint16_t rChan0LSB = 0X8A;

constexpr uint16_t rChan0MSB = 0X8B;

// Page 21
constexpr uint16_t rInterruptConfig = 0x8F;
constexpr uint8_t kIntPolarity = 0b00000'1'00; // Active high interrupt
constexpr uint8_t kIntDisabled = kIntPolarity + 0b000000'0'0;
constexpr uint8_t kIntEnabled = kIntPolarity + 0b000000'1'0;

// Page 22
constexpr uint16_t rIntThresholdHigh0 = 0x97;

constexpr uint16_t rIntThresholdLow0 = 0x99;

constexpr uint16_t rIntThresholdHigh1 = 0x98;

constexpr uint16_t rIntThresholdLow1  = 0x9A;

// Page 23
constexpr uint16_t rInterruptPersist = 0x9E;
constexpr uint8_t kIntPersist = 3; // 3 consecutive samples

// Other registers
constexpr int kNumCommRetries = 100;

/*
InterruptDeviation represents the denomiator for some simple math
Example: k3Percent = 32
low_threshold = value * (31 / 32) = value * 97%
high_threshold = value * (33 / 32) = value * 103%
*/
enum InterruptDeviation
{
  k25Percent = 4,
  k12Percent = 8,
  k6Percent  = 16,
  k3Percent  = 32,
  k1Percent  = 64
};
constexpr InterruptDeviation kInterruptDeviation = k3Percent;
} // namespace


Ltr_303als::Ltr_303als() {
  hI2c_ = I2cDevice(kBus, kAddr, kRegSize);
  last_brightness_ = 0;
  high_threshold_ = 0XFFFF;
  low_threshold_ = 0;

  HAL_StatusTypeDef status = HAL_OK;

  act_verifyPartId();
  act_swReset();
  setup_measureConfig();
  setup_interruptConfig();
  act_enableInterrupts();
  act_startSampling();

  status = this->act_setInterruptThresholds();
  if (status != HAL_OK) {Error_Handler();}
}

Ltr_303als::~Ltr_303als() {

}

void Task_Ltr_303als(void *argument) {
  ltr303als_event_flags = osEventFlagsNew(NULL);
  Ltr_303als ltr303als;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t flags;
  for(;;) {
    flags = osEventFlagsWait(ltr303als_event_flags, LTR303ALS_FLAG_INT, osFlagsWaitAny, osWaitForever);
    if (flags & LTR303ALS_FLAG_INT) {
      status = ltr303als.act_setInterruptThresholds();
      if (status != HAL_OK) {
        Error_Handler();
      }
      brightness = ltr303als.get_brightness();
    }
  }
}

void Ltr_303als::setup_measureConfig() {
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rMeasIntRate, kMeasAndIntRate);
  if (status != HAL_OK) {Error_Handler();}
}

void Ltr_303als::setup_interruptConfig() {
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rInterruptPersist, kIntPersist);
  if (status != HAL_OK) {Error_Handler();}
}

void Ltr_303als::act_disableInterrupts() {
  HAL_StatusTypeDef status = HAL_OK;
  act_stopSampling();

  status = hI2c_.act_pollVerifyWrite(rInterruptConfig, kIntDisabled);
  if (status != HAL_OK) {Error_Handler();}
}

void Ltr_303als::act_enableInterrupts() {
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rInterruptConfig, kIntEnabled);
  if (status != HAL_OK) {Error_Handler();}

  act_startSampling();
}

HAL_StatusTypeDef Ltr_303als::act_pollBrightness() {
  /*
  Internal comparisions for thresholds are with channel 0 and not 1.
  Channel 0 and 1 can have vastly different values.
  */
  uint8_t rx_buffer = 0;
  hI2c_.act_pollRead(rChan1LSB, &rx_buffer);
  // data = rx_buffer;
  hI2c_.act_pollRead(rChan1MSB, &rx_buffer);
  // last_brightness_ = data + (rx_buffer << 8);

  hI2c_.act_pollRead(rChan0LSB, &rx_buffer);
  last_brightness_ = rx_buffer;
  hI2c_.act_pollRead(rChan0MSB, &rx_buffer);
  last_brightness_ = last_brightness_ + (rx_buffer << 8);

  return HAL_OK;
}

HAL_StatusTypeDef Ltr_303als::act_setInterruptThresholds() {
  HAL_StatusTypeDef status = HAL_OK;
  status = this->act_pollBrightness();
  if (status != HAL_OK) {return status;}

  /*
  The following math is used to set an interrupt thereshold that is
  X% larger/smaller than current brightness.
  Uses divider that is power of 2 to avoid floating point math.
  Prevents overflow of the 16 bit number.
  */
  constexpr uint16_t kHigherNumerator = kInterruptDeviation + 1;
  constexpr uint16_t kMaxThreshold = 0xFFFF;
  constexpr uint16_t KMaxValueToOverflow = ((kMaxThreshold / kHigherNumerator) * kInterruptDeviation);
  if (last_brightness_ >= KMaxValueToOverflow) {
    high_threshold_ = kMaxThreshold;
  }
  else {
    high_threshold_ = ((last_brightness_ * kHigherNumerator) / kInterruptDeviation);
  }

  uint8_t high_threshold_msb = static_cast<uint8_t>((high_threshold_ >> 8) & 0xFF);
  status = hI2c_.act_pollVerifyWrite(rIntThresholdHigh1, high_threshold_msb);
  if (status != HAL_OK) {return status;}

  uint8_t high_threshold_lsb = static_cast<uint8_t>(high_threshold_ & 0xFF);
  status = hI2c_.act_pollVerifyWrite(rIntThresholdHigh0, high_threshold_lsb);
  if (status != HAL_OK) {return status;}

  constexpr uint16_t kLowerNumerator = kInterruptDeviation - 1;
  low_threshold_ = ((last_brightness_ * kLowerNumerator) / kInterruptDeviation);
  uint8_t low_threshold_msb = static_cast<uint8_t>((low_threshold_ >> 8) & 0xFF);
  status = hI2c_.act_pollVerifyWrite(rIntThresholdLow1, low_threshold_msb);
  if (status != HAL_OK) {return status;}

  uint8_t low_threshold_lsb = static_cast<uint8_t>(low_threshold_ & 0xFF);
  status = hI2c_.act_pollVerifyWrite(rIntThresholdLow0, low_threshold_lsb);
  if (status != HAL_OK) {return status;}

  if (high_threshold_ < low_threshold_) {
    Error_Handler();
  }

  return status;
}

void Ltr_303als::act_startSampling() {
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rControlSettings, kMaxGainAndActive);
  if (status != HAL_OK) {Error_Handler();}
}

void Ltr_303als::act_stopSampling() {
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollVerifyWrite(rControlSettings, kGainAndDeactive);
  if (status != HAL_OK) {Error_Handler();}
}

void Ltr_303als::act_swReset() {
  HAL_StatusTypeDef status = HAL_OK;
  status = hI2c_.act_pollWrite(rControlSettings, kSwReset);
  if (status != HAL_OK) {Error_Handler();}

  status = hI2c_.act_waitForResponse();
  if (status != HAL_OK) {Error_Handler();}
}

void Ltr_303als::act_verifyPartId() {
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t rx_buffer = 0;
  status = hI2c_.act_pollRead(rPartIdAndRev, &rx_buffer);
  if (status != HAL_OK) {Error_Handler();}
  // Current doesn't check revision, only part ID.
  if ((rx_buffer & kPartId) != kPartId) {Error_Handler();}
}