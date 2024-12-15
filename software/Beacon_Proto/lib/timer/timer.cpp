#include "timer.h"

Timer::Timer() {
    group = TIMER_GROUP_MAX;
    channel = TIMER_MAX;
}

Timer::~Timer() {

}

esp_err_t Timer::initiate(timer_group_t group, timer_idx_t channel) {
    constexpr uint32_t kOneMegaHerzFactor = 80;
    constexpr timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE, 
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP, 
        .auto_reload = TIMER_AUTORELOAD_EN,
        .clk_src = TIMER_SRC_CLK_APB, 
        .divider = kOneMegaHerzFactor // 80MHz / 80 = 1 MHz
    };
    esp_err_t err = timer_init(group, channel, &config);
    if (err != ESP_OK) {
        this->group = group;
        this->channel = channel;
    }
    return err;
}

esp_err_t Timer::set_alarm(uint64_t alarm_value_micro_seconds) {
    return timer_set_alarm_value(this->group, this->channel,
                                 alarm_value_micro_seconds);
}

esp_err_t Timer::action_start() {
    return timer_start(this->group, this->channel);
}

esp_err_t Timer::action_pause() {
    return timer_pause(this->group, this->channel);
}


