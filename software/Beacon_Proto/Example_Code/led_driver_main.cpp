#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "led_driver.h"

#include "reserved_objects.h"

extern "C" {void app_main();}

constexpr gpio_num_t GPIO_OUTPUT_IO_0  = GPIO_NUM_5;
constexpr uint64_t GPIO_OUTPUT_PIN_SEL = (1ULL << GPIO_OUTPUT_IO_0);


void heartbeatSetup() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void vHeartbeat(void * pvParameters) {

    heartbeatSetup();
    int cnt = 0;
    for( ;; ) {
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        cnt++;
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void app_main() {
    // xTaskCreatePinnedToCore(vHeartbeat, "Heartbeat", 8000, NULL, 1, NULL, 0);

    LedDriver greenLed = LedDriver();
    LedDriver redLed   = LedDriver();
    LedDriver blueLed  = LedDriver();

    greenLed.initiate(reserved_pin::kGreenStatusLed, LEDC_TIMER_3, LEDC_CHANNEL_0);
    redLed.initiate(reserved_pin::kRedStatusLed, LEDC_TIMER_3, LEDC_CHANNEL_1);
    blueLed.initiate(reserved_pin::kBlueStatusLed, LEDC_TIMER_3, LEDC_CHANNEL_2);
    
    for (;;) {
        greenLed.set_duty(300);
        redLed.set_duty(700);
        blueLed.set_duty(1000);
        greenLed.action_on();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        redLed.action_on();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        blueLed.action_on();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        greenLed.set_duty(5);
        greenLed.action_on();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        redLed.set_duty(10);
        redLed.action_on();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        blueLed.set_duty(100);
        blueLed.action_on();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        greenLed.action_off();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        redLed.action_off();
        vTaskDelay(1000/portTICK_PERIOD_MS);

        blueLed.action_off();
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}

