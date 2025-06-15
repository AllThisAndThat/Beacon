#pragma once

#include "main.h"
#include "cmsis_os2.h"

#include "is31fl3741.h"
#include "neopixel_4310.h"
#include "ano_rotary.h"
#include "ltr_303als.h"

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

#define NEOPIXEL4310_TIM_PWM_DONE   (1U << 3)
extern osEventFlagsId_t neopixel4310_tim_pwm_done;

#ifdef __cplusplus
namespace global_hardware {
extern is31fl3741::IS31FL3741 is31fl3741;
extern neopixel_4310::Neopixel4310 neopixel4310;
extern ano_rotary::AnoRotary ano_rotary;
extern ltr303als::Ltr_303als ltr303als;
} // namespace global_hardware
#endif
