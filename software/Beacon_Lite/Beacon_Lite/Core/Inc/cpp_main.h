#pragma once

#include "main.h"

typedef struct {
  I2C_HandleTypeDef *hi2c;
  TIM_HandleTypeDef *htim;
} handles_t;

#ifdef __cplusplus
extern "C" {
#endif

  void cpp_main(handles_t handles);

#ifdef __cplusplus
}
#endif