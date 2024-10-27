#define ESP32S3_DEVKITC_1 

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "pin_manager.h"
#include "digital_output_object.h"

extern "C"
{
    void app_main();
}

void app_main()
{
    PinManager *manager = manager->Instance();

    DigitalOutputObject blueLed = DigitalOutputObject();
    DigitalOutputConfig blueLedConfig =
        {
            .pinBitMask = (1ULL << GPIO_NUM_4),
            .mode = PUSH_PULL,
            .pullState = PULL_DOWN
        };
    blueLed.setConfig(blueLedConfig);

    manager->enableDigitalOutputObject(&blueLed);

    if (blueLed.isEnabled() == Enabled_t::ENABLED)
    {
        for (int i = 0; i < 10; i++)
        {
            blueLed.action_toggle();
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        abort();
    }
}
