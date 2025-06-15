/*
https://cdn-learn.adafruit.com/downloads/pdf/adafruit-seesaw-atsamd09-breakout.pdf
*/

#include "ano_rotary.h"

// #include "cmsis_os2.h"
#include "syscfg.h"

#include "cpp_main.h"

namespace ano_rotary {
namespace {
  // I2C Setup
  namespace I2c {
  constexpr uint16_t kAddr    = syscfg::i2c::addr::kAnoRotary;
  inline I2C_HandleTypeDef&  kBus     = syscfg::i2c::bus::kAnoRotary;
  constexpr RegSize  kRegSize = RegSize::k16Bit;
  }

  // Common Constants
  constexpr uint8_t kSetAllButtons = static_cast<uint8_t>(Button::kAll);

  // Base/High Registers
  constexpr uint16_t rStatusBase  = 0x00 << 8;
  constexpr uint16_t rGpioBase    = 0x01 << 8;
  constexpr uint16_t rEncoderBase = 0x11 << 8;

  // Status registers
  constexpr uint16_t rHwID = rStatusBase + 0x01;

  constexpr uint16_t rVersion = rStatusBase + 0x02;
  constexpr uint16_t kVersion = 5740;

  constexpr uint16_t rSWReset = rStatusBase + 0x7F;
  constexpr uint8_t  kSWReset = 0xFF;

  // GPIO registers
  constexpr uint16_t rInputSet = rGpioBase + 0x03;

  constexpr uint16_t rGpioState = rGpioBase + 0x04;

  constexpr uint16_t rPullUpSet = rGpioBase + 0x05;

  constexpr uint16_t rIntButtonSet    = rGpioBase + 0x08;
  constexpr uint8_t  kIntButtonSetAll = kSetAllButtons;

  constexpr uint16_t rIntButtonClr    = rGpioBase + 0x09; 
  constexpr uint8_t  kIntButtonClrAll = 0xFF;

  constexpr uint16_t rPullEnable = rGpioBase + 0x0B;

  // Encoder registers
  /*
  rIntEncoderSet is write only!
  */
  constexpr uint16_t rIntEncoderSet = rEncoderBase + 0x10;
  constexpr uint8_t  kIntEncoderSet = 0x01;

  constexpr uint16_t rEncoderPosition = rEncoderBase + 0x30;
} // namespace

AnoRotary::AnoRotary() {
}

void AnoRotary::init() {
  hI2c_ = I2cDevice(&I2c::kBus, I2c::kAddr, I2c::kRegSize);
  act_verify();  
  act_SWReset();
  act_enableEncoderInts();
  act_setButtonInputPU();
  act_enableButtonInts();

  ano_rotary_event_flags = osEventFlagsNew(NULL);

  isInit_ = true;
}

uint8_t AnoRotary::get_buttonState(Button button) const {
  return (buttonState_ & static_cast<uint8_t>(button)) ? 1 : 0;
}

void AnoRotary::act_waitForButtonRelease(Button button) {
  while (get_buttonState(button) == 0) {
    // wait for button press
    osDelay(10);
  }

  while (get_buttonState(button) == 1) {
    // wait for button release
    osDelay(10);
  }
}

void AnoRotary::act_enableEncoderInts() {
  HAL_StatusTypeDef status;
  status = hI2c_.act_pollWrite(rIntEncoderSet, kIntEncoderSet);
  if (status != HAL_OK) {Error_Handler();}
}

void AnoRotary::act_enableButtonInts() {
  HAL_StatusTypeDef status;

  // Clear all interrupts and then set desired interrupts. Potentially unnecessary after a SWReset
  constexpr size_t clrBuffSize = 4;
  uint8_t clrBuffer[clrBuffSize] = {0, 0, 0, kIntButtonClrAll};
  status = hI2c_.act_pollWrite(rIntButtonClr, clrBuffer, clrBuffSize);
  if (status != HAL_OK) {Error_Handler();}

  constexpr size_t setBuffSize = 4;
  uint8_t setBuffer[setBuffSize] = {0, 0, 0, kIntButtonSetAll};
  status = hI2c_.act_pollWrite(rIntButtonSet, setBuffer, setBuffSize);
  if (status != HAL_OK) {Error_Handler();}
}

void AnoRotary::act_readButtonState() {
  HAL_StatusTypeDef status;
  constexpr size_t rxBuffSize = 4;
  uint8_t rxBuff[rxBuffSize];
  status = hI2c_.act_pollRead(rGpioState, rxBuff, rxBuffSize);
  if (status != HAL_OK) {Error_Handler();}

  buttonState_ = (rxBuff[3] & static_cast<uint8_t>(Button::kAll));
}

void AnoRotary::act_readEncoderPosition() {
  HAL_StatusTypeDef status;
  constexpr size_t rxBuffSize = 4;
  uint8_t rxBuff[rxBuffSize];
  status = hI2c_.act_pollRead(rEncoderPosition, rxBuff, rxBuffSize); 
  if (status != HAL_OK) {Error_Handler();}

  encoderPosition_ = (static_cast<uint32_t>(rxBuff[0]) << 24) |
                     (static_cast<uint32_t>(rxBuff[1]) << 16) |
                     (static_cast<uint32_t>(rxBuff[2]) << 8)  |
                     (static_cast<uint32_t>(rxBuff[3]) << 0);
}

void AnoRotary::act_setButtonInputPU() {
  HAL_StatusTypeDef status;
  constexpr size_t setInputBuffSize = 4;
  uint8_t setInputBuff[setInputBuffSize] = {0, 0, 0, kSetAllButtons};
  status = hI2c_.act_pollWrite(rInputSet, setInputBuff, setInputBuffSize);
  if (status != HAL_OK) {Error_Handler();}
 
  constexpr size_t pullEnBuffSize = 4;
  uint8_t pullEnBuffer[pullEnBuffSize] = {0, 0, 0, kSetAllButtons};
  status = hI2c_.act_pollWrite(rPullEnable, pullEnBuffer, pullEnBuffSize);
  if (status != HAL_OK) {Error_Handler();}

  constexpr size_t setPuBuffSize = 4;
  uint8_t setPuBuff[setPuBuffSize] = {0, 0, 0, kSetAllButtons};
  status = hI2c_.act_pollWrite(rPullUpSet, setPuBuff, setPuBuffSize);
  if (status != HAL_OK) {Error_Handler();}
}

void AnoRotary::act_SWReset() {
  HAL_StatusTypeDef status;
  status = hI2c_.act_pollWrite(rSWReset, kSWReset);
  if (status != HAL_OK) {Error_Handler();}

  status = hI2c_.act_waitForResponse();
  if (status != HAL_OK) {Error_Handler();}
}

HAL_StatusTypeDef AnoRotary::act_verify() {
  constexpr size_t rxBuffSize = 2;
  uint8_t rxBuffer[rxBuffSize];
  hI2c_.act_pollRead(rVersion, rxBuffer, rxBuffSize);
  uint16_t retVal = ((uint16_t)rxBuffer[0] << 8) | ((uint16_t)rxBuffer[1] << 0);
  if (retVal != kVersion) {return HAL_ERROR;}

  return HAL_OK;
}

void Task_AnoEncoder(void *argument) {
  while(!global_hardware::ano_rotary.get_isInit()) {
    osDelay(10); // Wait for ltr303als to be initialized
  }
  
  uint32_t flags;
  for(;;) {
    flags = osEventFlagsWait(ano_rotary_event_flags, ANO_ROTARY_FLAG_INT, osFlagsWaitAny, osWaitForever);
    if (flags & ANO_ROTARY_FLAG_INT) {
      global_hardware::ano_rotary.act_readButtonState();
      global_hardware::ano_rotary.act_readEncoderPosition();
    }
  }
}
} // namespace ano_rotary