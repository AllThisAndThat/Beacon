#pragma once

#include "driver/timer.h"

/* 
Needs test file
Only resolution to 1 micro-second
*/

class Timer {
public:
    Timer();
    ~Timer();

    esp_err_t initiate(timer_group_t group, timer_idx_t id);

    esp_err_t set_alarm(uint64_t alarm_value);

    esp_err_t action_start();
    esp_err_t action_pause();


private:
    timer_group_t group;
    timer_idx_t channel;
};