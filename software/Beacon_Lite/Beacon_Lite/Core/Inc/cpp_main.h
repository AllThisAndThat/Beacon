#pragma once

#include "main.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif
  void cpp_main();
#ifdef __cplusplus
}
#endif

#define LTR303ALS_FLAG_INT   (1U << 0)
extern osEventFlagsId_t ltr303als_event_flags;

#define ANO_ROTARY_FLAG_INT   (1U << 1)
extern osEventFlagsId_t ano_rotary_event_flags;

#define I2C_DMA_FLAG_DONE   (1U << 2)
extern osEventFlagsId_t i2c_dma_done_event_flags;