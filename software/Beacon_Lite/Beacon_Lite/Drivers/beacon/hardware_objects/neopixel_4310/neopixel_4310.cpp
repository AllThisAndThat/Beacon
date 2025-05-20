#include "neopixel_4310.h"

#include "main.h"

int datasentflag = 0;

Neopixel4310::Neopixel4310() {
  color_ = 0;
  set_color(color_);
}

Neopixel4310::~Neopixel4310() {

}

void Neopixel4310::set_colorHSL(uint8_t h, uint8_t s, uint8_t l) {
  // https://stackoverflow.com/questions/13105185/fast-algorithm-for-rgb-hsl-conversion
  if (l == 0) {
    color_ = 0;
  }
  else {
    uint16_t l1 = l + 1;
    uint8_t c;
    if (l < 128) {
      c = ((l1 << 1) * s) >> 8;
    }
    else {
      c = (512 - (l1 << 1)) * s >> 8;
    }

    uint16_t H = h * 6;              // 0 to 1535 (actually 1530)
    uint8_t lo = H & 255;           // Low byte  = primary/secondary color mix
    uint16_t h1 = lo + 1;
    uint8_t x;

    if ((H & 256) == 0) {
      x = h1 * c >> 8;   // even sextant, like red to yellow
    }        
    else {
      x = (256 - h1) * c >> 8;  // odd sextant, like yellow to green
    }

    uint8_t m = l - (c >> 1);
    uint8_t  r, g, b ;
    switch(H >> 8) {       // High byte = sextant of colorwheel
    case 0 : r = c; g = x; b = 0; break; // R to Y
    case 1 : r = x; g = c; b = 0; break; // Y to G
    case 2 : r = 0; g = c; b = x; break; // G to C
    case 3 : r = 0; g = x; b = c; break; // C to B
    case 4 : r = x; g = 0; b = c; break; // B to M
    default: r = c; g = 0; b = x; break; // M to R
    }

    color_ = (((uint32_t)r + m) << 16) | (((uint32_t)g + m) << 8) | ((uint32_t)b + m);
  }
  set_pwmData(color_);
}

void Neopixel4310::set_color(uint32_t color) {
  color_ = color;
  set_pwmData(color_);
}

void Neopixel4310::act_blank() {
  this->set_color(0);
  this->act_refresh();
}

void Neopixel4310::act_refresh()
{
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)pwmData_, kPwmDataSize);
	while (!datasentflag){};
	datasentflag = 0;
}

void Neopixel4310::set_pwmData(uint32_t color) {
  for (int j = 0; j < kNumPixels; j++) {
    for (int i = 0; i < kLedsPerPixel * 8; i++) {
      if (color & (1 << ((kLedsPerPixel * 8 - 1) - i))) {
        pwmData_[j * (kLedsPerPixel * 8) + i] = kPwmHigh;
      }
      else {
        pwmData_[j * (kLedsPerPixel * 8) + i] = kPwmLow;
      }
    }
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    /* Need to initialize this to zero so there isn't a glitch on the
       next cycle.
    */
    htim2.Instance->CCR1 = 0; 
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
    datasentflag = 1;
  }
}