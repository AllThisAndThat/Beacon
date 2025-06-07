
#pragma once

#include "stm32h5xx_hal.h"
#include "main.h"

#define BREADBOARD
// #define PCBV1_1

#ifdef BREADBOARD
namespace syscfg
{
namespace i2c {
  namespace addr {
    // Bus 1
    constexpr uint16_t kIs31fl3741 = 0x30 << 1; // ADDR = GND

    // Bus 2
    constexpr uint16_t kLtr303als = 0x29 << 1;
    constexpr uint16_t kAnoRotary = 0x49 << 1;
  }
  namespace bus {
    inline I2C_HandleTypeDef& kIs31fl3741 = hi2c1;

    inline I2C_HandleTypeDef& kLtr303als = hi2c2;
    inline I2C_HandleTypeDef& kAnoRotary = hi2c2;
    
  }

} // namespace i2c

namespace is31fl3741 {
  enum NumRows : uint8_t {
    k2 = 0b0111,
    k3 = 0b0110,
    k4 = 0b0101,
    k5 = 0b0100,
    k6 = 0b0011,
    k7 = 0b0010,
    k8 = 0b0001,
    k9 = 0b0000
  };
  enum LogicThresholds : uint8_t {
    k1V4_0V4 = 0b0,
    k2V4_0V6 = 0b1
  };
  enum DeghostResistor : uint8_t {
    kNone   = 0b000,
    k1KOhm  = 0b010,
    k2KOhm  = 0b011,
    k4KOhm  = 0b100,
    k8KOhm  = 0b101,
    k16KOhm = 0b110,
    k32KOhm = 0b111
  };
  
  constexpr NumRows kNumChannelScan = NumRows::k9;
  constexpr LogicThresholds kLogicThresholds = LogicThresholds::k2V4_0V6;
  constexpr DeghostResistor kDeghostResistor = DeghostResistor::k8KOhm;
} // namespace is31fl3741

} // namespace syscfg
#endif