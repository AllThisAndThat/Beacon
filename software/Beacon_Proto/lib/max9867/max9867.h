#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "driver/i2s_std.h"

#include "i2c.h"

/*
Max9867 Terminology:
For single ended inputs only (there are also microphone inputs)
- Playback - path from input to headphone amplifier
- Record - sampled by ADC
- DC Tracking - can you ADC to sample DC voltages
- Capacitorless ouptut - might have the advantage of more power
  delivery and higher impedance headphones
- Left/right preamp - amplifier for single ended inputs
- Input Mix - can mix single-ended inputs and microphone inputs
- Output Mix - mix both DACs and playback signals
- Low pass and high pass filtering available
- Sidetone - mix record and playback signals after digital filtering (DAC)
- AUX ADC - adc designed to sample dc voltage. Can also sample audio same time.
- NI value can be used so you don't need to provide LRCLK or BCLK 
- Should calibrate for DC measurements
*/

enum class JackState : uint8_t {
    kPlugged   = 0,
    kUnplugged = 1
};

class Max9867 {
public:
    Max9867();
    ~Max9867();
    esp_err_t initiate();

    esp_err_t get_hp_jack_state(JackState& state);

    esp_err_t set_left_adc_gain(int gain);
    esp_err_t set_left_linein_gain(int gain);
    esp_err_t set_dac_gain(int gain);
    esp_err_t set_headphone_level(int gain);

    esp_err_t action_probe();
    esp_err_t action_verify();
    esp_err_t action_on();
    esp_err_t action_off();
    esp_err_t action_mute();
    esp_err_t action_unmute();

    esp_err_t test_read_audio();
    esp_err_t test_echo_audio();

private:
    I2c i2c_;
    i2c_master_dev_handle_t hDevice_;
    i2s_chan_handle_t hI2sRx_;
    i2s_chan_handle_t hI2sTx_;

    TaskHandle_t hTask_;

    uint8_t dacGain_;

    esp_err_t setup_i2c();
    esp_err_t setup_mode();
    esp_err_t setup_system_clock();
    esp_err_t setup_interface_mode();
    esp_err_t setup_master_i2s();
    esp_err_t setup_adc_input();
    esp_err_t setup_audio_filter();

    static void vTask(void *pvParameters);
};