/*

*/
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {

namespace beacon_math {

typedef struct {
  uint8_t h;
  uint8_t s;
  uint8_t l;
} HslColor;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RgbColor;

extern RgbColor hslToRgb(HslColor hsl);
} // namespace beacon_math

} // extern "C"
#endif