#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
// #include "driver/gptimer.h"


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

// namespace reserved_LTR_303 {
//     constexpr bool ENABLED = true;
//     constexpr uint16_t kAddr = 0x29;

//     extern i2c_device_config_t kDeviceCfg;
// }

namespace reserved_LTR_303 {
    constexpr bool ENABLED = true;
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