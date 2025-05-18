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

/*
Todo: try to make inline function in i2c to write a value and then read is saved. For ltr_303als initiate.
*/