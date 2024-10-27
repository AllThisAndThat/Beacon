
#include "pwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr uint32_t MAX_DUTY = 1000;

openClose_t PWM::driverProtection = OPEN;

static void pwm_task(void *pvParameters);

PWM::PWM() 
{
    this->duty = 0;
    this->pin = GPIO_NUM_NC;
    this->defaultLogic = LOW;
    this->instanceAccess = CLOSED;
    this->state = STOP;
}

PWM::~PWM() 
{
    this->close();
}

openClose_t PWM::open() 
{
    if (driverProtection == OPEN)
    {
        driverProtection = CLOSED;
        this->instanceAccess = OPEN;

        this->gpio_setup();
        xTaskCreate(pwm_task, "pwm_task", 2048, this, 10, NULL);
        
    }
    return this->instanceAccess;
}

void PWM::close()
{
    if (instanceAccess == OPEN)
    {
        this->instanceAccess = CLOSED;
        driverProtection = OPEN;
    }
}

uint32_t PWM::getDuty() const
{
    return this->duty;
}

gpio_num_t PWM::getPin() const
{
    return this->pin;
}

logic_t PWM::getDefaultLogic() const
{
    return this->defaultLogic;
}

state_t PWM::getState() const
{
    return this->state;
}

error_t PWM::setAll(uint32_t duty, gpio_num_t pin, logic_t defaultLogic)
{

    this->defaultLogic = defaultLogic;
    setPin(pin);
    return setDuty(duty);
}

error_t PWM::setDuty(uint32_t duty)
{
    error_t check = checkDuty(duty);
    if (check == NO_ERROR)
    {
        this->duty = duty;
    }
    return check;
}

void PWM::setPin(gpio_num_t pin)
{
    //should add checking later
    this->pin = pin;
}

error_t PWM::writeState(state_t state)
{
    error_t check = NO_ERROR;
    if (this->instanceAccess == OPEN)
    {
        this->state = state;
    }
    else
    {
        check = ERROR;
    }
    return check;
}

error_t PWM::checkDuty(uint32_t duty)
{
    if (duty > MAX_DUTY)
    {
        return ERROR;
    }
    return NO_ERROR;
}

void PWM::gpio_setup()
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL << this->pin);
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    gpio_set_level(this->pin, this->defaultLogic);
}

static void pwm_task(void* pvParameters)
{
    PWM *instance = static_cast<PWM*>(pvParameters);
    gpio_num_t pin = instance->getPin();
    volatile esp_err_t err;
    for (;;) {
        if (instance->getState() == RUNNING)
        {
            err = gpio_toggle_level(pin);
        }
        vTaskDelay(instance->getDuty() / portTICK_PERIOD_MS);
    }
}