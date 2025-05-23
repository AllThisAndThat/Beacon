#include "cpp_main.h"

#include "is31fl3741.h"

HAL_StatusTypeDef status = HAL_OK;


HslColor hsl = {0, 200, 30};
uint32_t row = 0;
uint32_t col = 0;

uint32_t elapsed_ms = 0;

void cpp_main() {
  IS31FL3741 is31fl3741;
  
  is31fl3741.set_globalCurrent(0x01);
  if (status != HAL_OK) { Error_Handler();}

  is31fl3741.act_on();
  if (status != HAL_OK) { Error_Handler();}

  is31fl3741.act_refreshBrightness();

  uint32_t start;
  uint32_t end;
  for (;;) {
    
    is31fl3741.set_pixel(row, col, hsl);
    if (status != HAL_OK) { Error_Handler();}


    hsl.h += 1;
    col++;
    if (col > 12) {
      col = 0;
      row++;
      if (row > 8) {
        row = 0;
      }
    }
    start = HAL_GetTick();
    is31fl3741.act_refreshColor();
    if (status != HAL_OK) { Error_Handler();}
    end = HAL_GetTick();
    
    elapsed_ms = end - start;

    // HAL_Delay(10);
  }

}