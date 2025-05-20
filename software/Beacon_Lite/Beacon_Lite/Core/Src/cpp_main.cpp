#include "cpp_main.h"

#include "neopixel_4310.h"

Neopixel4310 neopixel;
uint8_t hue = 0;

void cpp_main() {
  
  
  
  neopixel.act_refresh();

  
  uint8_t saturation = 127;
  uint8_t lightness = 127;
  uint8_t hue = 0;

  for(;;) {
    neopixel.set_colorHSL(hue, saturation, lightness);
    neopixel.act_refresh();
    hue += 1;
    HAL_Delay(50);

    if (hue == 255) {
      neopixel.act_blank();
      while(1);
    }
    
  }

}