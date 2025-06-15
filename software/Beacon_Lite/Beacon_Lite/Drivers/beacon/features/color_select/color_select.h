/*

*/
#pragma once

namespace color_select {
#ifdef __cplusplus
extern "C" {
#endif

void Task_ColorSelect(void *argument);

#ifdef __cplusplus
}
#endif

class ColorSelect {
 public:
  ColorSelect();

  friend void Task_ColorSelect(void *argument);

 private:
};

} // namespace color_select