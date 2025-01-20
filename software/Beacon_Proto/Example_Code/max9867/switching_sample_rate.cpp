#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h" // TODO: delete me


#include "reserved_objects.h"
#include "status_led.h"

// Custom includes
#include "max9867.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}
void failure();
void warning();
void pass();

StatusLed statusLed = StatusLed();

// Custom instances
Max9867 max9867 = Max9867();

void app_main() {
    esp_err_t err;

    statusLed.initiate();
    statusLed.set_color(Color::kGreen);
    statusLed.set_state(StatusLedState::kBlink);

// Custom initiates
    err = max9867.initiate();
    if (err != ESP_OK) {
        failure();
    }

    // err = max9867.action_verify();
    // if (err != ESP_OK) {
    //     failure();
    // }

    // err = max9867.action_on();
    // if (err != ESP_OK) {
    //     failure();
    // }

    // max9867.set_left_adc_gain(3);
    // max9867.set_left_linein_gain(24);

    i2s_std_clk_config_t ClockCfg = {
        .sample_rate_hz = 16'000, // For Max9867 -> 8 to 48 kHz
        .clk_src = I2S_CLK_SRC_DEFAULT, //I2S_CLK_SRC_DEFAULT
        .mclk_multiple = I2S_MCLK_MULTIPLE_768 // 256 -> 48kHz * 256 = 12.288MHz
    }; 

    int cycles = 500;
    constexpr uint32_t kMaxBlockTime = 10000;
    size_t bytes_read;
    for (;;) {
        err = i2s_channel_disable(max9867.hI2sRx_);
        if (err != ESP_OK) {failure();}
        i2s_channel_reconfig_std_clock(max9867.hI2sRx_, &ClockCfg);
        if (err != ESP_OK) {failure();}
        else {pass();}
        vTaskDelay(100/portTICK_PERIOD_MS);
        err = i2s_channel_enable(max9867.hI2sRx_);
        if (err != ESP_OK) {failure();}
        printf("%ld Hz\n", ClockCfg.sample_rate_hz);
        for (int i = 0; i < cycles; i++) {
            constexpr int kBufferSize = 512;
            static int16_t buffer[kBufferSize]; // Buffer to store the data
            i2s_channel_read(max9867.hI2sRx_, buffer, kBufferSize, &bytes_read, kMaxBlockTime);
            i2s_channel_write(max9867.hI2sTx_, buffer, bytes_read, NULL, kMaxBlockTime);
        }

        if (ClockCfg.sample_rate_hz >= 48'000) {
            ClockCfg.sample_rate_hz = 16'000;
            ClockCfg.mclk_multiple  = I2S_MCLK_MULTIPLE_768;
        }
        else{
            ClockCfg.sample_rate_hz += 8'000;
            ClockCfg.mclk_multiple = static_cast<i2s_mclk_multiple_t>(12288 / (ClockCfg.sample_rate_hz / 1000));
            printf("MCLK Multiple: %d\n", ClockCfg.mclk_multiple);
        }
        // err = max9867.test_echo_audio();
        // if (err != ESP_OK) {warning();}
        // else {pass();}
    }
}

void failure() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kRed);
    printf("Failure\n");
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void warning() {
    statusLed.set_state(StatusLedState::kBlink);
    statusLed.set_color(Color::kYellow);
}

void pass() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kGreen);
}