#pragma once

#include "stm32h5xx_hal.h"

namespace syscfg
{
  namespace pin {
    
  }

  namespace port {
    // constexpr auto kStatusRed = GPIOA;
  }

  namespace i2c1 {
    namespace addr {
      constexpr uint16_t kLtr303als = 0x29 << 1;
    }

  }
}  