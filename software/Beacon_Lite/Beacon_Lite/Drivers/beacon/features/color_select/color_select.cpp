/*
IDEAS:
  * add functions to wait until button is released
  * Setup tasks for IS31FL3741 and adafruit5201 to refresh at a fixed rate
*/
#include "color_select.h"

#include "cmsis_os2.h"

#include "cpp_main.h"

namespace color_select {

namespace {
  enum Mode {
    kIdle = 0,
    kSelect,
    
  };
}
// ColorSelect::ColorSelect() {
// }

void Task_ColorSelect(void *argument) {
  global_hardware::neopixel4310.init();
  global_hardware::ano_rotary.init();
  // global_hardware::is31fl3741.init();
  global_hardware::ltr303als.init();

  // global_hardware::is31fl3741.set_globalCurrent(0x02);
  // global_hardware::is31fl3741.act_on();

  beacon_math::HslColor hsl = {0, 255, 0x80};
  int delay = 20;
  uint8_t buttonState = 0;
  uint32_t encoderPosition = 0;
  Mode mode = kIdle;
  for(;;) {
    global_hardware::ano_rotary.act_waitForButtonRelease(ano_rotary::Button::kSelect);

    while (1) {
      encoderPosition = global_hardware::ano_rotary.get_encoderPosition();
      hsl.h = encoderPosition % 256;
      global_hardware::neopixel4310.set_colorHSL(hsl);
      osDelay(delay);
    }
  }
}
} // namespace color_select