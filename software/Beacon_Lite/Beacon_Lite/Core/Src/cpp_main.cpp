#include "cpp_main.h"

#include "is31fl3741.h"

HAL_StatusTypeDef status = HAL_OK;

void cpp_main() {
  // I2c i2c1 (hi2c1);
  // uint8_t rx_buffer = 0;
  // uint16_t addr = 0x30 << 1;
  // uint8_t idReg = 0xFC;
  // status = i2c1.act_pollRead(addr, idReg, &rx_buffer);

  IS31FL3741 is31fl3741;

  is31fl3741.set_globalCurrent(0x01);
  if (status != HAL_OK) { Error_Handler();}

  is31fl3741.act_on();
  if (status != HAL_OK) { Error_Handler();}

  LedConfig config = {0, 0, 0, 0, 0};
  uint8_t period = 1;
  uint8_t increment = 10;

  for (;;) {
    increment += 5;
    for (int color = 0; color < 3; color++) {
      for (uint8_t x = 0; x < 13; x++) {
        if (color == 0) {
          config.r += increment;
        }
        else if (color == 1) {
          config.b += increment;
        }
        else {
          config.g += increment;
        }
        config.x = x;
        for (uint8_t y = 0; y < 9; y++) {
          config.y = y;
          status = is31fl3741.set_pixel(config);
          if (status != HAL_OK) {
            Error_Handler();
          }
          HAL_Delay(period);

        }
      }
    }
}

}