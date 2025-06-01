#include "cpp_main.h"

#include "cmsis_os2.h"


void cpp_main() {

}

osEventFlagsId_t ltr303als_event_flags;
osEventFlagsId_t ano_rotary_event_flags;

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == AMB_INT_Pin) {
    osEventFlagsSet(ltr303als_event_flags, LTR303ALS_FLAG_INT);
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == ANO_INT_Pin) {
    osEventFlagsSet(ano_rotary_event_flags, ANO_ROTARY_FLAG_INT);
  }
}