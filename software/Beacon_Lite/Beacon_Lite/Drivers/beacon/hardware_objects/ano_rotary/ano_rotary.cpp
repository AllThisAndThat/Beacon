#include "ano_rotary.h"

#include "syscfg.h"

namespace {
// I2C Setup
constexpr uint16_t kAddr    = syscfg::i2c::addr::kAnoRotary;
inline I2C_HandleTypeDef&  kBus     = syscfg::i2c::bus::kAnoRotary;
constexpr RegSize  kRegSize = RegSize::k16Bit;

// Common Constants
constexpr uint8_t kSetAllButtons = static_cast<uint8_t>(Button::kAll);

// Base/High Registers
constexpr uint16_t rStatusBase  = 0x00 << 8;
constexpr uint16_t rGpioBase    = 0x01 << 8;
constexpr uint16_t rIntBase     = 0x0B << 8; // TODO: delete if not used
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
constexpr uint16_t rIntEncoderSet = rEncoderBase + 0x10;
constexpr uint8_t  kIntEncoderSet = 0x01;

constexpr uint16_t rEncoderPosition = rEncoderBase + 0x30;

} // namespace

AnoRotary::AnoRotary()
  : hI2c_(kBus, kAddr, kRegSize) {

  this->act_verify();
  this->act_SWReset();
  HAL_Delay(1); // Wait for reset to complete

  this->act_enableEncoderInts();
  this->act_setButtonInputPU();
  this->act_enableButtonInts();
}

AnoRotary::~AnoRotary() {

}

ButtonState AnoRotary::get_buttonState(Button button) {
  HAL_StatusTypeDef status;
  constexpr size_t rxBuffSize = 4;
  uint8_t rxBuff[rxBuffSize];
  status = hI2c_.act_pollRead(rGpioState, rxBuff, rxBuffSize);
  if (status != HAL_OK) {this->error_handler();}

  ButtonState val = ButtonState::kNotPresed;
  if ((rxBuff[3] & static_cast<uint8_t>(button)) == 0) {
    val = ButtonState::kPressed; // Button is pressed
  }
  
  return val;
  // Assume we only need first byte
  // uint8_t rxBuff;
  // status = hI2c_.act_pollRead(rGpioState, &rxBuff);
  // if (status != HAL_OK) {this->error_handler();}
  
  // return rxBuff; //TODO: Fix
}

uint32_t AnoRotary::get_encoderPosition() {
  HAL_StatusTypeDef status;
  constexpr size_t rxBuffSize = 4;
  uint8_t rxBuff[rxBuffSize];
  status = hI2c_.act_pollRead(rEncoderPosition, rxBuff, rxBuffSize); 
  if (status != HAL_OK) {this->error_handler();}

  return (uint32_t)rxBuff[0] << 24 | ((uint32_t)rxBuff[1] << 16) | ((uint32_t)rxBuff[2] << 8) | ((uint32_t)rxBuff[3] << 0);
  //TODO: interpret the result and return appropriate value. Do all 4 bytes hold useful data? Do I need to collect them into a uint32_t?
}

void AnoRotary::act_enableEncoderInts() {
  HAL_StatusTypeDef status;

  status = hI2c_.act_pollWrite(rIntEncoderSet, kIntEncoderSet);
  if (status != HAL_OK) {this->error_handler();}
}

void AnoRotary::act_enableButtonInts() {
  HAL_StatusTypeDef status;

  // Clear all interrupts and then set desired interrupts. Potentially unnecessary after a SWReset
  constexpr size_t clrBuffSize = 4;
  uint8_t clrBuffer[clrBuffSize] = {0, 0, 0, kIntButtonClrAll};
  status = hI2c_.act_pollWrite(rIntButtonClr, clrBuffer, clrBuffSize);
  if (status != HAL_OK) {this->error_handler();}

  constexpr size_t setBuffSize = 4;
  uint8_t setBuffer[setBuffSize] = {0, 0, 0, kIntButtonSetAll};
  status = hI2c_.act_pollWrite(rIntButtonSet, setBuffer, setBuffSize);
  if (status != HAL_OK) {this->error_handler();}
}

void AnoRotary::act_setButtonInputPU() {
  /*
 pinModeBulk(1ul << pin, mode);
 1 << 1 = 0b0011_1110
 1 << 2
 1 << 3
 1 << 4
 1 << 5

    this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
    this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
    this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);

    this->write(1 << 8 = 256, 0x03 = 3, cmd, 4); 259
    this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4); 256+ 11 = 267
    this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4); 256 + 5 = 261
    break;
*/
  HAL_StatusTypeDef status;
  constexpr size_t setInputBuffSize = 4;
  uint8_t setInputBuff[setInputBuffSize] = {0, 0, 0, kSetAllButtons};
  status = hI2c_.act_pollWrite(rInputSet, setInputBuff, setInputBuffSize); //SEESAW_GPIO_DIRCLR_BULK 0x03
  if (status != HAL_OK) {this->error_handler();}
 
  constexpr size_t pullEnBuffSize = 4;
  uint8_t pullEnBuffer[pullEnBuffSize] = {0, 0, 0, kSetAllButtons};
  status = hI2c_.act_pollWrite(rPullEnable, pullEnBuffer, pullEnBuffSize);
  if (status != HAL_OK) {this->error_handler();}

  constexpr size_t setPuBuffSize = 4;
  uint8_t setPuBuff[setPuBuffSize] = {0, 0, 0, kSetAllButtons};
  status = hI2c_.act_pollWrite(rPullUpSet, setPuBuff, setPuBuffSize);
  if (status != HAL_OK) {this->error_handler();}
}

void AnoRotary::act_SWReset() {
  HAL_StatusTypeDef status;
  status = hI2c_.act_pollWrite(rSWReset, kSWReset);
  if (status != HAL_OK) {this->error_handler();}
}

void AnoRotary::act_verify() {
  constexpr size_t rxBuffSize = 2;
  uint8_t rxBuffer[rxBuffSize];
  hI2c_.act_pollRead(rVersion, rxBuffer, rxBuffSize);
  uint16_t retVal = ((uint16_t)rxBuffer[0] << 8) | ((uint16_t)rxBuffer[1] << 0);
  if (retVal != kVersion) {this->error_handler();}
}

void AnoRotary::error_handler () {
  // TODO: Hold LED RED
  for(;;) {

  }
}