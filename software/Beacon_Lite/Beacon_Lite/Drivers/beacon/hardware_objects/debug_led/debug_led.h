#pragma once

#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_tim.h"

enum class Color
{
    kRed,
    kGreen,
    kBlue,
    kYellow
};

class DebugLed {
public:
    DebugLed(TIM_HandleTypeDef hTim);
    ~DebugLed();
    HAL_StatusTypeDef initiate();

    HAL_StatusTypeDef set_color(Color color);
    HAL_StatusTypeDef set_color(uint16_t r, uint16_t g, uint16_t b);

    HAL_StatusTypeDef action_start();
    HAL_StatusTypeDef action_stop();

private:
    TIM_HandleTypeDef hTim_;
    // uint32_t duty_;
};