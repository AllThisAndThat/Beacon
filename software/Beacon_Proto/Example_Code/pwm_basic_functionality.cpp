/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "driver\gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "pwm.h"


extern "C"
{
    void app_main();
}


void app_main()
{
    PWM pwm;
    error_t check;
    openClose_t openClose;
    check = pwm.setAll(50, GPIO_NUM_4, LOW);

    openClose = pwm.open();

    pwm.writeState(RUNNING);




    for (;;)
    {
        printf("main\n"); 
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
