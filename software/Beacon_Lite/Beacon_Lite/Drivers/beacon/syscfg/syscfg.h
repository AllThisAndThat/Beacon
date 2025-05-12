#pragma once

#include "stm32h5xx_hal.h"

namespace syscfg
{
  namespace pin {
    constexpr uint16_t kStatusRed = GPIO_PIN_8;
  }

  namespace port {
    // constexpr auto kStatusRed = GPIOA;
  }
}  