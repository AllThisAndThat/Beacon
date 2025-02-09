#include "is31fl3741.h"

namespace IS31FL3741_Config {
    // Page 14
    constexpr uint8_t kSWS_n9        = (0b0000 << 4);
    constexpr uint8_t kLGC_2V4_0V6   = (1 << 3);
    constexpr uint8_t kOSDE_noDetect = (0b00 << 1);
    
    constexpr uint8_t kData = kSWS_n9 | kLGC_2V4_0V6 | kOSDE_noDetect;
    constexpr uint8_t kDataOff = kData | (0 << 0);
    constexpr uint8_t kDataOn  = kData | (1 << 0);
    constexpr uint8_t rRegister = 0x00;
}

IS31FL3741::IS31FL3741()
  : i2c_(reserved::IS31FL3741::kI2cPort) {
    page_ = Page::k4;
    state_ = IS31FL3741_State::kOff;
    global_current_ = 0x00;
}

IS31FL3741::~IS31FL3741() {

}

esp_err_t IS31FL3741::initiate() {
    using namespace reserved::IS31FL3741;

    esp_err_t err = i2c_.initiate();
    if (err != ESP_OK) {return err;}

    err = i2c_.action_add_device(reserved::IS31FL3741::kDeviceCfg,
                                 hDevice_);
    if (err != ESP_OK) {return err;}

    err = setup_config_register();
    if (err != ESP_OK) {return err;}

    err = setup_pull_resistor();
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t IS31FL3741::set_global_current(const uint8_t current) {
    esp_err_t err = set_page(Page::k4);
    if (err != ESP_OK) {return err;}

    global_current_ = current;

    constexpr uint8_t rGlobalCurrent = 0x01;
    return i2c_.action_write_reg(hDevice_, rGlobalCurrent, current);
}

esp_err_t IS31FL3741::set_specific_led(uint32_t led_num,
                                       const uint8_t pwm,
                                       const uint8_t scale) {
    constexpr uint32_t kMaxLedNum = 351; 
    if (led_num >= kMaxLedNum) {return ESP_ERR_INVALID_ARG;}
    
    esp_err_t err = ESP_OK;
    constexpr uint32_t kGroup1Max = 180;
    if (led_num < kGroup1Max) {
        err = set_page(Page::k0);
        if (err != ESP_OK) {return err;}
        err = i2c_.action_write_reg(hDevice_, led_num, pwm);
        if (err != ESP_OK) {return err;}

        err = set_page(Page::k2);
        if (err != ESP_OK) {return err;}
        err = i2c_.action_write_reg(hDevice_, led_num, scale);
    } 
    else {
        led_num -= kGroup1Max;
        err = set_page(Page::k1);
        if (err != ESP_OK) {return err;}
        err = i2c_.action_write_reg(hDevice_, led_num, pwm);
        if (err != ESP_OK) {return err;}

        err = set_page(Page::k3);
        if (err != ESP_OK) {return err;}
        err = i2c_.action_write_reg(hDevice_, led_num, scale);
    }
    return err;
}

esp_err_t IS31FL3741::set_specific_led(uint8_t x, uint8_t y,
                                       uint8_t color, uint8_t brightness) {
    constexpr uint8_t kMaxX = 12;
    constexpr uint8_t kMaxY = 8;
    if ((x > kMaxX) || (y > kMaxY)) {
        return ESP_ERR_INVALID_ARG;
    }

    constexpr uint8_t rowmap[] = {8, 5, 4, 3, 2, 1, 0, 7, 6};
    y = rowmap[y];

    uint16_t offset = (x + ((x < 10) ? (y * 10) : (80 + y * 3))) * 3;
    constexpr uint16_t rOffset = 2;
    constexpr uint16_t gOffset = 1;
    constexpr uint16_t bOffset = 0;
    if ((x & 1) || (x == 12)) {
        constexpr uint8_t remap[] = {2, 0, 1};
        switch (color) {
            case 0:
                set_specific_led((offset + remap[rOffset]), brightness, brightness);
                break;
            case 1:
                set_specific_led((offset + remap[gOffset]), brightness, brightness);
                break;
            case 2:
                set_specific_led((offset + remap[bOffset]), brightness, brightness);
                break;
        }
    }
    else {
        switch (color) {
        case 0:
            set_specific_led((offset + rOffset), brightness, brightness);
            break;
        case 1:
            set_specific_led((offset + gOffset), brightness, brightness);
            break;
        case 2:
            set_specific_led((offset + bOffset), brightness, brightness);
            break;
        }
    }

    return ESP_OK;
}

esp_err_t IS31FL3741::action_verify() {
    constexpr uint8_t addr = reserved::i2c1::kAddr_IS31FL3741;
    esp_err_t err = i2c_.action_probe(addr);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kCorrectId = 0x60;
    constexpr uint8_t rId = 0xFC;
    uint8_t kReadId;
    err = i2c_.action_read(hDevice_, rId, &kReadId);
    if (err != ESP_OK) {return err;}

    if (kReadId != kCorrectId) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return err;
}

esp_err_t IS31FL3741::action_off() {
    esp_err_t err = set_page(Page::k4);
    if (err != ESP_OK) {return err;}

    using namespace IS31FL3741_Config;
    err = i2c_.action_write_reg(hDevice_, rRegister, kDataOff);
    if (err != ESP_OK) {return err;}

    state_ = IS31FL3741_State::kOff;

    return err;
}

esp_err_t IS31FL3741::action_on() {
    esp_err_t err = set_page(Page::k4);
    if (err != ESP_OK) {return err;}

    using namespace IS31FL3741_Config;
    err = i2c_.action_write_reg(hDevice_, rRegister, kDataOn);
    if (err != ESP_OK) {return err;}

    state_ = IS31FL3741_State::kOn;

    return err;
}

esp_err_t IS31FL3741::action_reset_all_leds() {
    esp_err_t err = set_page(Page::k4);
    if (err != ESP_OK) {return err;}

    // Reset all registers
    constexpr uint8_t rReset = 0x3F;
    constexpr uint8_t kReset = 0xAE;
    err = i2c_.action_write_reg(hDevice_, rReset, kReset);
    if (err != ESP_OK) {return err;}

    // Restore non-led registers
    err = setup_config_register();
    if (err != ESP_OK) {return err;}
    err = setup_pull_resistor();
    if (err != ESP_OK) {return err;}
    err = set_global_current(global_current_);
    if (err != ESP_OK) {return err;}

    if (state_ == IS31FL3741_State::kOn) {
        return action_on();
    }

    return err;
}

esp_err_t IS31FL3741::setup_config_register() {
    esp_err_t err = set_page(Page::k4);
    if (err != ESP_OK) {return err;}

    using namespace IS31FL3741_Config;
    return i2c_.action_write_reg(hDevice_, rRegister, kDataOff);
}

esp_err_t IS31FL3741::setup_pull_resistor() {
    esp_err_t err = set_page(Page::k4);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kPDR_32k = (0b111 << 4);
    constexpr uint8_t kPUR_32k = (0b111 << 0);
    constexpr uint8_t kPullResistor = kPDR_32k | kPUR_32k;
    constexpr uint8_t rPullResistor = 0x02;
    return i2c_.action_write_reg(hDevice_, rPullResistor, kPullResistor);
}

esp_err_t IS31FL3741::set_page(const Page page) {
    if (page == page_) {return ESP_OK;}

    page_ = page;

    constexpr uint8_t kUnlock = 0b1100'0101;
    constexpr uint8_t rUnlock = 0xFE;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rUnlock, kUnlock);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t rPageSelect = 0xFD;
    return i2c_.action_write_reg(hDevice_, rPageSelect, 
                                 static_cast<uint8_t>(page));
}

esp_err_t IS31FL3741::action_write_page(const Page page,
                                        uint8_t* data,
                                        const size_t data_len) {
    esp_err_t err = set_page(page);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t rInitialAddress = 0x00;
    return i2c_.action_write_multi(hDevice_, rInitialAddress, data,
                                   data_len);
}
