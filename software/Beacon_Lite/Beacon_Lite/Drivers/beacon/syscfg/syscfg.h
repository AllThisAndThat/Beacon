
#pragma once

#include "stm32h5xx_hal.h"
#include "main.h"

namespace syscfg
{
  namespace i2c {
    namespace addr {
      // Bus 1
      constexpr uint16_t kIs31fl3741 = 0x30 << 1; // ADDR = GND

      // Bus 2
      constexpr uint16_t kLtr303als = 0x29 << 1;
    }
    namespace bus {
      inline I2C_HandleTypeDef& kIs31fl3741 = hi2c1;

      inline I2C_HandleTypeDef& kLtr303als = hi2c2;
      
    }

  }
}  