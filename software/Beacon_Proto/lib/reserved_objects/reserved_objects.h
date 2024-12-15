#include "driver/gpio.h"
#include "driver/ledc.h"
// #include "driver/gptimer.h"


namespace reserved_pin {
    constexpr int kUart0Tx = 43;
    constexpr int kUart0Rx = 44;

    constexpr gpio_num_t kLedDriverTestLed = GPIO_NUM_21;

    constexpr gpio_num_t kGreenStatusLed = GPIO_NUM_5;
    constexpr gpio_num_t kRedStatusLed   = GPIO_NUM_4;
    constexpr gpio_num_t kBlueStatusLed  = GPIO_NUM_6;
}

namespace reserved_ledc {
    constexpr ledc_timer_t kStatusLeds = LEDC_TIMER_0;

    constexpr ledc_channel_t kGreenLed = LEDC_CHANNEL_0;
    constexpr ledc_channel_t kRedLed   = LEDC_CHANNEL_1;
    constexpr ledc_channel_t kBlueLed  = LEDC_CHANNEL_2;
}

// namespace reserved_gptimer {
//     constexpr timer_group_t kBlinkGroup   = TIMER_GROUP_0;
//     constexpr timer_idx_t   kBlinkChannel = TIMER_0;
// }
