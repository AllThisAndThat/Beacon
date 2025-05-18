#include "cpp_main.h"

#include "debug_led.h"

void cpp_main() {
  DebugLed debugled(htim1);

  debugled.action_start();
  debugled.set_color(Color::kRed);
  
  // for(;;) {

  // }

}