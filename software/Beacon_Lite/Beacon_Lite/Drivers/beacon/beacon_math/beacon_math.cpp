/*

*/
#include "beacon_math.h"

namespace beacon_math {
RgbColor hslToRgb(HslColor hsl) {
// https://stackoverflow.com/questions/13105185/fast-algorithm-for-rgb-hsl-conversion
  RgbColor rgb;
  if (hsl.l == 0) {
    return rgb ={0, 0, 0};
  }

  uint16_t l1 = hsl.l + 1;
  uint8_t c;
  if (hsl.l < 128) {
    c = ((l1 << 1) * hsl.s) >> 8;
  }
  else {
    c = (512 - (l1 << 1)) * hsl.s >> 8;
  }

  uint16_t H = hsl.h * 6;              // 0 to 1535 (actually 1530)
  uint8_t lo = H & 255;           // Low byte  = primary/secondary color mix
  uint16_t h1 = lo + 1;
  uint8_t x;

  if ((H & 256) == 0) {
    x = h1 * c >> 8;   // even sextant, like red to yellow
  }        
  else {
    x = (256 - h1) * c >> 8;  // odd sextant, like yellow to green
  }

  uint8_t m = hsl.l - (c >> 1);
  uint8_t  r, g, b ;
  switch(H >> 8) {       // High byte = sextant of colorwheel
  case 0 : r = c; g = x; b = 0; break; // R to Y
  case 1 : r = x; g = c; b = 0; break; // Y to G
  case 2 : r = 0; g = c; b = x; break; // G to C
  case 3 : r = 0; g = x; b = c; break; // C to B
  case 4 : r = x; g = 0; b = c; break; // B to M
  default: r = c; g = 0; b = x; break; // M to R
  }

  rgb.r = r + m;
  rgb.g = g + m;
  rgb.b = b + m;
  return rgb;
}
} // namespace beacon_math