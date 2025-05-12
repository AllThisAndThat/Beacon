#pragma once

#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_tim.h"

class Timer {
public:
    Timer();
    ~Timer();
    HAL_StatusTypeDef initiate();

    HAL_StatusTypeDef set_color(uint32_t r, uint32_t g, uint32_t b);

    HAL_StatusTypeDef action_start();
    HAL_StatusTypeDef action_stop();

private:
    TIM_HandleTypeDef htim_;
    // uint32_t duty_;
};