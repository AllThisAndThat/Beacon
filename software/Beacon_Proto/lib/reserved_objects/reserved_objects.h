#pragma once

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"

#include "i2c.h"
#include "ltr_303als.h"
#include "i2s.h"
// #include "driver/gptimer.h"

/*
Pins

I2C0 addresses
I2C1 addresses

Task to which core

enable/disable features
*/


namespace reserved_pin {
    constexpr int kUart0Tx = 43;
    constexpr int kUart0Rx = 44;

    constexpr gpio_num_t kLedDriverTestLed = GPIO_NUM_21;

    constexpr gpio_num_t kGreenStatusLed = GPIO_NUM_5;
    constexpr gpio_num_t kRedStatusLed   = GPIO_NUM_4;
    constexpr gpio_num_t kBlueStatusLed  = GPIO_NUM_6;

    constexpr int kSpi2Mosi = 11;
    constexpr int kSpi2Miso = 13;
    constexpr int kSpi2Sclk = 12;

    constexpr gpio_num_t kI2c0Sda = GPIO_NUM_39;
    constexpr gpio_num_t kI2c0Scl = GPIO_NUM_40;

    constexpr gpio_num_t kI2c1Sda = GPIO_NUM_45;
    constexpr gpio_num_t kI2c1Scl = GPIO_NUM_0;
    constexpr gpio_num_t kI2c1Int = GPIO_NUM_35;
}

namespace reserved_ledc {
    constexpr ledc_timer_t kStatusLeds = LEDC_TIMER_0;

    constexpr ledc_channel_t kGreenLed = LEDC_CHANNEL_0;
    constexpr ledc_channel_t kRedLed   = LEDC_CHANNEL_1;
    constexpr ledc_channel_t kBlueLed  = LEDC_CHANNEL_2;
}



// SPI2 has IO MUX which offers less delay and supports higher speeds
namespace reserved_spi2 {

    // Bus Config
    constexpr int kNotUsed = -1;
    constexpr spi_bus_config_t kBusCfg = {
            .mosi_io_num     = reserved_pin::kSpi2Mosi,
            .miso_io_num     = reserved_pin::kSpi2Miso,
            .sclk_io_num     = reserved_pin::kSpi2Sclk,
            .data2_io_num    = kNotUsed,
            .data3_io_num    = kNotUsed,
            .data4_io_num    = kNotUsed,
            .data5_io_num    = kNotUsed,
            .data6_io_num    = kNotUsed,
            .data7_io_num    = kNotUsed,
            // .max_transfer_sz = 0,
            // .flags           = 0,
            .isr_cpu_id      = ESP_INTR_CPU_AFFINITY_AUTO,
            // .intr_flags      = 0
        };
    
    // Device Config
    constexpr uint8_t kPolarity0Phase0 = 0;
    constexpr uint16_t k50PercentDuty = 128;
    constexpr spi_device_interface_config_t kDeviceCfg = {
        .command_bits     = 0,
        .address_bits     = 8,
        .dummy_bits       = 8, // 8 = spi mode
        .mode             = kPolarity0Phase0,
        .clock_source     = SPI_CLK_SRC_APB,
        .duty_cycle_pos   = k50PercentDuty,
        // .cs_ena_pretrans  = 0,
        // .cs_ena_posttrans = 0,
        .clock_speed_hz   = 10'000'000,
        // .input_delay_ns   = 0,
        // .spics_io_num     = 10,
        // .flags            = 0,
        // .queue_size       = 8,
        // .pre_cb           = 0,
        // .post_cp          = 0,
    };

    // Transaction Config
    constexpr size_t tx_length = 16;
    constexpr size_t rx_length = 16;
}

namespace reserved_i2c0 {
    constexpr uint32_t kSclSpeedHz = 400'000;
    constexpr i2c_master_bus_config_t kBusCfg = {
        .i2c_port = 0,
        .sda_io_num = reserved_pin::kI2c0Sda,
        .scl_io_num = reserved_pin::kI2c0Scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = 1}
    };
}

namespace reserved_i2c1 {
    constexpr uint32_t kSclSpeedHz = 400'000;
    constexpr i2c_master_bus_config_t kBusCfg = {
        .i2c_port = 1,
        .sda_io_num = reserved_pin::kI2c1Sda,
        .scl_io_num = reserved_pin::kI2c1Scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = 1}
    };
    
    constexpr gpio_num_t kIntPin = reserved_pin::kI2c1Int;
    constexpr gpio_config_t kIntCfg = {
        .pin_bit_mask = (1ULL << kIntPin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
}

namespace reserved_LTR_303 {
    constexpr I2cPort kI2cPort = I2cPort::kPort0;
    constexpr bool ENABLED = true; // Implement later - maybe have an enabled area
    constexpr uint16_t kAddr = 0x29;

    constexpr i2c_device_config_t kDeviceCfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = kAddr,
        .scl_speed_hz    = reserved_i2c0::kSclSpeedHz,
        .scl_wait_us     = 0,
        .flags = {
            .disable_ack_check = 0
        }
    };
}

namespace reserved_KTS1622 {
    constexpr I2cPort kI2cPort = I2cPort::kPort1;
    constexpr bool ENABLED = true;
    constexpr uint16_t kAddr = 0x20; // configurable with a pin polarity - 0x20 = VSS

    constexpr i2c_device_config_t kDeviceCfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = kAddr,
        .scl_speed_hz    = reserved_i2c0::kSclSpeedHz,
        .scl_wait_us     = 0,
        .flags = {
            .disable_ack_check = 0
        }
    };

    constexpr i2c_device_config_t kDeviceRstCfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = 0,
        .scl_speed_hz    = reserved_i2c0::kSclSpeedHz,
        .scl_wait_us     = 0,
        .flags = {
            .disable_ack_check = 0
        }
    };
}

namespace reserved_i2s0 {
    /*
    Add I2S0 pins to top
    Verify
        * sample rate
        * MCLK multiple
        * Data width
        * what is slot?
        * ws width
        * any inverted?
        * 
    */

    constexpr i2s_std_clk_config_t kClockCfg = {
        .sample_rate_hz = 44'100,
        .clk_src = I2S_CLK_SRC_DEFAULT,
        .mclk_multiple = I2S_MCLK_MULTIPLE_256 // 256 -> 44.1kHz * 256 = 11.2896MHz
    };

    constexpr bool kLowLevelFirst = false;
    constexpr i2s_std_slot_config_t kSlotCfg = {
        .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
        .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
        .slot_mode      = I2S_SLOT_MODE_MONO,
        .slot_mask      = I2S_STD_SLOT_LEFT,
        .ws_width       = 16,
        .ws_pol         = kLowLevelFirst,
        .bit_shift      = true,
        .left_align     = true,
        .big_endian     = false,
        .bit_order_lsb  = false
    };

    constexpr uint32_t kNotInverted = 0;
    constexpr i2s_std_gpio_config_t kGpioCfg = {
        .mclk = 0,
        .bclk = 0,
        .ws   = 0,
        .dout = 0,
        .din  = 0,
        .invert_flags = {
            .mclk_inv = kNotInverted,
            .bclk_inv = kNotInverted,
            .ws_inv = kNotInverted
        }      
    };

    constexpr i2s_std_config_t kChannelCfg = {
        .clk_cfg  = kClockCfg,
        .slot_cfg = kSlotCfg,
        .gpio_cfg = kGpioCfg,
    };
}








// SPI3 doesn't have IO MUX, uses gpio matrix
// namespace reserved_spi3 {
//     constexpr int kMosi    = 11;
//     constexpr int kMiso    = 13;
//     constexpr int kSclk    = 12;
//     constexpr int kNotUsed = -1;
//     constexpr int kCs      = 10; 
// }

// namespace reserved_gptimer {
//     constexpr timer_group_t kBlinkGroup   = TIMER_GROUP_0;
//     constexpr timer_idx_t   kBlinkChannel = TIMER_0;
// }