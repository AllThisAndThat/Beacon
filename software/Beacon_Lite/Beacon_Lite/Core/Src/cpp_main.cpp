#include "cpp_main.h"

#include "cmsis_os2.h"

#include "ano_rotary.h"

uint32_t encoderPosition;
ButtonState select_button;
ButtonState up_button;
ButtonState down_button;
ButtonState left_button;
ButtonState right_button;

void cpp_main() {
  AnoRotary ano;

  for (;;) {
    encoderPosition = ano.get_encoderPosition();
    select_button = ano.get_buttonState(Button::kSelect);
    up_button = ano.get_buttonState(Button::kUp);
    down_button = ano.get_buttonState(Button::kDown);
    left_button = ano.get_buttonState(Button::kLeft);
    right_button = ano.get_buttonState(Button::kRight);
    HAL_Delay(20);
  }
}