#pragma once

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "driver/uart.h"

// #include "driver/gptimer.h"

/*
enable/disable features
*/

enum class I2cPort {
    kPort0,
    kPort1
};

namespace reserved {

    namespace pin {
        constexpr int kUart0Tx = 43;
        constexpr int kUart0Rx = 44;

        constexpr gpio_num_t kLedDriverTestLed = GPIO_NUM_21;

        constexpr gpio_num_t kGreenStatusLed = GPIO_NUM_5;
        constexpr gpio_num_t kRedStatusLed   = GPIO_NUM_4;
        constexpr gpio_num_t kBlueStatusLed  = GPIO_NUM_6;

        constexpr int kSpi2Cs   = GPIO_NUM_10;
        constexpr int kSpi2Sclk = GPIO_NUM_12;
        constexpr int kSpi2Mosi = GPIO_NUM_13;
        constexpr int kSpi2Miso = GPIO_NUM_11;


        constexpr gpio_num_t kI2c0Sda = GPIO_NUM_39;                            
        constexpr gpio_num_t kI2c0Scl = GPIO_NUM_40;

        constexpr gpio_num_t kI2c1Sda = GPIO_NUM_45;
        constexpr gpio_num_t kI2c1Scl = GPIO_NUM_0;
        constexpr gpio_num_t kI2c1Int = GPIO_NUM_35;

        constexpr gpio_num_t kI2s0Mclk = GPIO_NUM_7;
        constexpr gpio_num_t kI2s0Bclk = GPIO_NUM_15;
        constexpr gpio_num_t kI2s0Ws   = GPIO_NUM_16;
        constexpr gpio_num_t kI2s0Dout = GPIO_NUM_17;
        constexpr gpio_num_t kI2s0Din  = GPIO_NUM_18;
    }

    namespace core {
        constexpr int kStatusLed = 0;
        constexpr int kBMI323 = 0;

        constexpr int kMax9867 = 1;
    }

    namespace priority {
        constexpr UBaseType_t kStatusLed = 1;
        constexpr UBaseType_t kBMI323 = 10;
        constexpr UBaseType_t kMax9867 = configMAX_PRIORITIES - 1;
    }

// Wrappers

    namespace i2c0 {
        constexpr uint8_t kAddr_Max9867 = 0x18;
        constexpr uint8_t kAddr_LTR_303 = 0x29;        

        constexpr uint32_t kSclSpeedHz = 400'000;
        constexpr i2c_master_bus_config_t kBusCfg = {
            .i2c_port = 0,
            .sda_io_num = pin::kI2c0Sda, 
            .scl_io_num = pin::kI2c0Scl,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1}
        };
    }

    namespace i2c1 {
        constexpr uint8_t kAddr_KTS1622_rst = 0x00;
        constexpr uint8_t kAddr_KTS1622     = 0x20;
        constexpr uint8_t kAddr_IS31FL3741  = 0x30; // ADDR = GND
        
        constexpr uint32_t kSclSpeedHz = 400'000;
        constexpr i2c_master_bus_config_t kBusCfg = {
            .i2c_port = 1,
            .sda_io_num = pin::kI2c1Sda,
            .scl_io_num = pin::kI2c1Scl,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1}
        };
        
        constexpr gpio_num_t kIntPin = pin::kI2c1Int;
        constexpr gpio_config_t kIntCfg = {
            .pin_bit_mask = (1ULL << kIntPin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_POSEDGE,
        };
    }

    namespace spi2 {
    // SPI2 has IO MUX which offers less delay and supports higher speeds

        constexpr spi_host_device_t kSpiNum = SPI2_HOST;

        // Bus Config
        constexpr spi_bus_config_t kBusCfg = {
            .mosi_io_num     = pin::kSpi2Mosi,
            .miso_io_num     = pin::kSpi2Miso,
            .sclk_io_num     = pin::kSpi2Sclk,
            .data2_io_num    = GPIO_NUM_NC,
            .data3_io_num    = GPIO_NUM_NC,
            .data4_io_num    = GPIO_NUM_NC,
            .data5_io_num    = GPIO_NUM_NC,
            .data6_io_num    = GPIO_NUM_NC,
            .data7_io_num    = GPIO_NUM_NC,
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,
            .flags           = SPICOMMON_BUSFLAG_MASTER,
        };
        

        // Device Config
        constexpr uint8_t kPolarity0Phase0 = 0;
        constexpr uint16_t k50PercentDuty = 128;
        constexpr spi_device_interface_config_t kDeviceCfg = {
            .mode             = kPolarity0Phase0,
            .clock_source     = SPI_CLK_SRC_APB,
            .duty_cycle_pos   = k50PercentDuty,
            .clock_speed_hz   = SPI_MASTER_FREQ_10M,
            .spics_io_num     = pin::kSpi2Cs,
            // .flags = SPI_DEVICE_HALFDUPLEX,
            .queue_size       = 7,

        };
    }

    namespace i2s0 {
        constexpr i2s_chan_config_t kChanCfg = {
            .id = I2S_NUM_0,
            .role = I2S_ROLE_MASTER,
            .dma_desc_num = 3,
            .dma_frame_num = 384,
            .auto_clear = true,
            .auto_clear_before_cb = false,
            .intr_priority = 0,
        };

        constexpr i2s_std_clk_config_t kClockCfg = {
            .sample_rate_hz = 48'000, // For Max9867 -> 8 to 48 kHz
            .clk_src = I2S_CLK_SRC_DEFAULT, //I2S_CLK_SRC_DEFAULT
            .mclk_multiple = I2S_MCLK_MULTIPLE_256 // 256 -> 48kHz * 256 = 12.288MHz
        };

        // 16 bits, mono, left slot only, left-align, non-philips mode, MSB first
        constexpr i2s_std_slot_config_t kSlotCfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = I2S_STD_SLOT_LEFT,
            .ws_width = I2S_DATA_BIT_WIDTH_16BIT,
            .ws_pol = false,
            .bit_shift = false,
            .left_align = true,
            .big_endian = false,
            .bit_order_lsb = false
        };

        constexpr uint32_t kNotInverted = 0;
        constexpr i2s_std_gpio_config_t kGpioCfg = {
            .mclk = pin::kI2s0Mclk,
            .bclk = pin::kI2s0Bclk,
            .ws   = pin::kI2s0Ws,
            .dout = pin::kI2s0Dout,
            .din  = pin::kI2s0Din,
            .invert_flags = {
                .mclk_inv = kNotInverted,
                .bclk_inv = kNotInverted,
                .ws_inv   = kNotInverted
            }      
        };

        constexpr i2s_std_config_t kStdCfg = {
            .clk_cfg  = kClockCfg,
            .slot_cfg = kSlotCfg,
            .gpio_cfg = kGpioCfg,
        };  
    }

    namespace uart0 {
        constexpr uart_port_t kUartNum = UART_NUM_0;

        constexpr int kTxPin  = pin::kUart0Tx;
        constexpr int kRxPin  = pin::kUart0Rx;
        constexpr int kRtsPin = -1;
        constexpr int kCtsPin = -1;

        constexpr uart_config_t kConfig = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 122,
            .source_clk = UART_SCLK_DEFAULT,
            .flags = {.backup_before_sleep = 0}
        };
    }

    namespace ledc {
        constexpr ledc_timer_t kStatusLeds = LEDC_TIMER_0;

        constexpr ledc_channel_t kGreenLed = LEDC_CHANNEL_0;
        constexpr ledc_channel_t kRedLed   = LEDC_CHANNEL_1;
        constexpr ledc_channel_t kBlueLed  = LEDC_CHANNEL_2;
    }

// Hardware Objects

    namespace LTR_303 {
        constexpr I2cPort kI2cPort = I2cPort::kPort0;
        constexpr bool ENABLED = true; // Implement later - maybe have an enabled area

        constexpr i2c_device_config_t kDeviceCfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address  = i2c0::kAddr_LTR_303,
            .scl_speed_hz    = i2c0::kSclSpeedHz,
            .scl_wait_us     = 0,
            .flags = {
                .disable_ack_check = 0
            }
        };
    }

    namespace KTS1622 {
        constexpr I2cPort kI2cPort = I2cPort::kPort1;
        constexpr bool ENABLED = true;

        constexpr i2c_device_config_t kDeviceCfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address  = i2c1::kAddr_KTS1622,
            .scl_speed_hz    = i2c0::kSclSpeedHz,
            .scl_wait_us     = 0,
            .flags = {
                .disable_ack_check = 0
            }
        };

        constexpr i2c_device_config_t kDeviceRstCfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address  = i2c1::kAddr_KTS1622_rst,
            .scl_speed_hz    = i2c0::kSclSpeedHz,
            .scl_wait_us     = 0,
            .flags = {
                .disable_ack_check = 0
            }
        };
    }

    namespace IS31FL3741 {
        constexpr I2cPort kI2cPort = I2cPort::kPort1;
        constexpr bool kIsEnabled = true;

        constexpr i2c_device_config_t kDeviceCfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address  = i2c1::kAddr_IS31FL3741,
            .scl_speed_hz    = i2c1::kSclSpeedHz,
            .scl_wait_us     = 0,
            .flags = {
                .disable_ack_check = 0
            }
        };
    }

    namespace max9867 {
        constexpr I2cPort kI2cPort = I2cPort::kPort0;
        constexpr bool kIsEnabled = true;

        constexpr i2c_device_config_t kDeviceCfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address  = i2c0::kAddr_Max9867,
            .scl_speed_hz    = i2c0::kSclSpeedHz,
            .scl_wait_us     = 0,
            .flags = {
                .disable_ack_check = 0
            }
        };
    }

    namespace BMI323 {
        constexpr spi_host_device_t port = SPI2_HOST;
    }
}
