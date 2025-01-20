#include "max9867.h"

#include <unordered_map>

#include "reserved_objects.h"


// #include "esp_timer.h"


constexpr uint8_t rDacGain = 0x0C;

Max9867::Max9867() 
    : i2c_(I2c(reserved_max9867::kI2cPort)) {
    // default muted
    dacGain_ =  0b0100'0000; 
}

Max9867::~Max9867() {

}

esp_err_t Max9867::initiate() {
    esp_err_t err;
    err = setup_i2c();
    if (err != ESP_OK) {return err;}

    err = setup_mode();
    if (err != ESP_OK) {return err;}

    err = setup_system_clock();
    if (err != ESP_OK) {return err;}

    err = setup_interface_mode();
    if (err != ESP_OK) {return err;}

    err = setup_adc_input();
    if (err != ESP_OK) {return err;}

    err = setup_audio_filter();
    if (err != ESP_OK) {return err;}

    err = setup_master_i2s();
    if (err != ESP_OK) {return err;}

    constexpr configSTACK_DEPTH_TYPE kStackSize = 20'000;
    constexpr UBaseType_t kTaskPriority = configMAX_PRIORITIES - 1; // Might want to set priority in reservered_objects
    xTaskCreatePinnedToCore(static_cast<TaskFunction_t>(this->vTask), 
                "audio", kStackSize, this, kTaskPriority,
                &hTask_, 1);
    return err;
}

esp_err_t Max9867::get_hp_jack_state(JackState& state) {
    constexpr uint8_t rJackSense = 0x01;
    uint8_t rx_buffer;
    esp_err_t err = i2c_.action_read(hDevice_, rJackSense,
                                     &rx_buffer);
    if (err != ESP_OK) {return err;}

    state = static_cast<JackState>((rx_buffer >> 7));
    return ESP_OK;
}

esp_err_t Max9867::set_left_adc_gain(int gain) {
    constexpr int kMaxGain = 3;
    constexpr int kMinGain = -12;
    if (gain > kMaxGain || gain < kMinGain) {
        return ESP_ERR_INVALID_ARG;
    }

    // Matches gain to respective register value, page 36
    uint8_t kAdcGain = ((static_cast<uint8_t>((-gain) + 3)) << 4);
    constexpr uint8_t rAdcGain = 0x0D;
    return i2c_.action_write_reg(hDevice_, rAdcGain, kAdcGain);
}

esp_err_t Max9867::set_left_linein_gain(int gain) {
    constexpr int kMaxGain = 24;
    constexpr int kMinGain = -6;
    if (gain > kMaxGain || gain < kMinGain) {
        return ESP_ERR_INVALID_ARG;
    }

    // Matches gain to respective register value, rounds down odd number, page 37
    uint8_t kLIGL = static_cast<uint8_t>(((-gain) + 24) / 2);
    constexpr uint8_t rLIGL = 0x0E;
    return i2c_.action_write_reg(hDevice_, rLIGL, kLIGL);
}

esp_err_t Max9867::set_dac_gain(int gain) {
    constexpr int kMaxGain = 17;
    constexpr int kMinGain = -15;
    if (gain > kMaxGain || gain < kMinGain) {
        return ESP_ERR_INVALID_ARG;
    }

    // Matches gain to respective register value, page 36
    uint8_t DACG = 0;
    if (gain > 0) {
        DACG = (static_cast<uint8_t>((gain / 6) + 1));
        gain = gain - (DACG * 6);
    }
    DACG = DACG << 4;
    uint8_t DACA = static_cast<uint8_t>(-(gain));

    uint8_t previous_mute_setting = (dacGain_ & (1 << 6));
    dacGain_ = previous_mute_setting | DACG | DACA;
    return i2c_.action_write_reg(hDevice_, rDacGain, dacGain_);
}

esp_err_t Max9867::set_headphone_level(int gain) {
    constexpr int kMaxGain = 0;
    constexpr int kMinGain = -39;
    if (gain > kMaxGain || gain < kMinGain) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t VOLLR = static_cast<uint8_t>(-gain);

    constexpr uint8_t rLeftVolume = 0x10;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rLeftVolume, VOLLR);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t rRightVolume = 0x11;
    err = i2c_.action_write_reg(hDevice_, rRightVolume, VOLLR);
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t Max9867::action_probe() {
    return i2c_.action_probe(reserved_max9867::kAddr);
}

esp_err_t Max9867::action_verify() {
    uint8_t rx_buffer = 0x00;
    constexpr uint8_t rRevision = 0xFF;
    esp_err_t err = i2c_.action_read(hDevice_, rRevision,
                                     &rx_buffer);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kCorrectRevision = 0x42;
    if (rx_buffer != kCorrectRevision) {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t Max9867::action_on() {
    // Start muted to prevent clicking
    esp_err_t err = action_mute();
    if (err != ESP_OK) {return err;}

    // Only supports left line to left adc to both left and right dac
    constexpr uint8_t kNSHDN_On       = (1 << 7);
    constexpr uint8_t kLNLEN_disable  = (0 << 6);
    constexpr uint8_t kLNREN_disable  = (0 << 5);
    constexpr uint8_t kDALEN_enable   = (1 << 3);
    constexpr uint8_t kDAREN_enable   = (1 << 2);
    constexpr uint8_t kADLEN_enable   = (1 << 1);
    constexpr uint8_t kADREN_disable  =  0;
    constexpr uint8_t kSysShutdown = kNSHDN_On | kLNLEN_disable |
                                     kLNREN_disable | kDALEN_enable |
                                     kDAREN_enable | kADLEN_enable |
                                     kADREN_disable;
    constexpr uint8_t rSysShutdown = 0x17;
    err = i2c_.action_write_reg(hDevice_, rSysShutdown,
                                          kSysShutdown);
    return err;
}

esp_err_t Max9867::action_off() {
    constexpr uint8_t kAllOff = 0;
    constexpr uint8_t rSysShutdown = 0x17;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rSysShutdown,
                                          kAllOff);
    return err;
}

esp_err_t Max9867::action_mute() {
    dacGain_ = dacGain_ | (1 << 6);
    return i2c_.action_write_reg(hDevice_, rDacGain, dacGain_);
}

esp_err_t Max9867::action_unmute() {
    // Set mute bit to unmute
    dacGain_ = dacGain_ & ~(1 << 6);
    return i2c_.action_write_reg(hDevice_, rDacGain, dacGain_);
}

esp_err_t Max9867::test_read_audio() {
/*
interrupt_interval = 4 ms
dma_frame_num = interrupt_interval * sample_rate = 0.004 * 48'000 = 192
dma_buffer_size = dma_frame_num * slot_num * data_bit_width / 8 = 192 * 1 * 16 / 8 = 384 
dma_desc_num > polling_cycle / interrupt_interval = 10 / 4 -> 3
recv_buffer_size > dma_desc_num * dma_buffer_size = 384 * 3 = 1152
*/

    constexpr uint32_t kMaxBlockTime = 50;
    size_t bytes_read;
    constexpr int kBufferSize = 1152;
    static int16_t buffer[kBufferSize]; // Buffer to store the data

    esp_err_t err = i2s_channel_read(hI2sRx_, buffer, kBufferSize, &bytes_read, kMaxBlockTime);
    if (err != ESP_OK) {return err;}

    printf("Read %d bytes from I2S\n", bytes_read);

    // for (int i = 0; i < (bytes_read / 2); i++) {
    //     printf(">ADC: %d\n", buffer[i]);
    // }

    return err;
}

esp_err_t Max9867::test_echo_audio() {
    constexpr uint32_t kMaxBlockTime = 100;
    size_t bytes_read, bytes_written;
    constexpr int kBufferSize = 1152;
    static int16_t buffer[kBufferSize]; // Buffer to store the data
    esp_err_t err = i2s_channel_read(hI2sRx_, buffer, kBufferSize, &bytes_read, kMaxBlockTime);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_write(hI2sTx_, buffer, bytes_read, &bytes_written, kMaxBlockTime);
    if (err != ESP_OK) {return err;}

    if (bytes_read != bytes_written) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t Max9867::setup_i2c() {
    esp_err_t err = i2c_.initiate();
    if (err != ESP_OK) {return err;}

    err = i2c_.action_add_device(reserved_max9867::kDeviceCfg,
                                 hDevice_);
    if (err != ESP_OK) {return err;}

    return ESP_OK;
}

esp_err_t Max9867::setup_mode() {
    constexpr uint8_t kDSLEW_slow           = (1 << 7);
    constexpr uint8_t kNVSEN_smooth         = (0 << 6);
    constexpr uint8_t kNZDEN_zeroCrossing   = (0 << 5);
    constexpr uint8_t kJDETEN_disable       = (0 << 3); //TODO: try to detect headphones later
    constexpr uint8_t kHPMODE_stereoClickless = (0b100); 
    constexpr uint8_t kMode = kDSLEW_slow | kNVSEN_smooth
                            | kNZDEN_zeroCrossing | kJDETEN_disable
                            | kHPMODE_stereoClickless;
    constexpr uint8_t rMode = 0x16;
    return i2c_.action_write_reg(hDevice_, rMode, kMode);
}

esp_err_t Max9867::setup_system_clock() {
    constexpr uint8_t kPSCLK_10to20 = (0b01 << 4); // TODO: Connect to MCLK reserved somehow
    constexpr uint8_t kFREQ_normal  =  0b0000;
    constexpr uint8_t kSystemClk = (kPSCLK_10to20 | kFREQ_normal);
    constexpr uint8_t rSystemClk = 0x05;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rSystemClk,
                                          kSystemClk);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kPLL_externalLR = (1 << 7);
    constexpr uint8_t rAudioClkHigh = 0x06;
    err = i2c_.action_write_reg(hDevice_, rAudioClkHigh,
                                kPLL_externalLR);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kNoNi = 0;
    constexpr uint8_t rAudioClkLow = 0x07;
    err = i2c_.action_write_reg(hDevice_, rAudioClkLow, kNoNi);
    if (err != ESP_OK) {return err;}

    return ESP_OK;
}

esp_err_t Max9867::setup_interface_mode() {
    // Page 26
    constexpr uint8_t kMAS_slaveMode     = 0 << 7;
    constexpr uint8_t kWCI_noInvert      = 0 << 6;
    constexpr uint8_t kBCI_risingEdge    = 0 << 5;
    constexpr uint8_t kDLY_leftJustified = 0 << 4;
    constexpr uint8_t kHIZOFF_noHiz      = 1 << 3;
    constexpr uint8_t kTDM_off           = 0 << 2;
    constexpr uint8_t kInterface1a = kMAS_slaveMode     | 
                                     kWCI_noInvert      |
                                     kBCI_risingEdge    |
                                     kDLY_leftJustified |
                                     kHIZOFF_noHiz      |
                                     kTDM_off;
    constexpr uint8_t rInterface1a = 0x08;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rInterface1a,
                                          kInterface1a);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kLVOLFIX_track  = (0 << 4);
    constexpr uint8_t kDMONO_mono     = (1 << 3);
    constexpr uint8_t kBSEL_notUsed  =  0b000;
    constexpr uint8_t kInterface1b = kLVOLFIX_track | 
                                     kDMONO_mono    |
                                     kBSEL_notUsed;
    constexpr uint8_t rInterface1b = 0x09;                                   
    err = i2c_.action_write_reg(hDevice_, rInterface1b,
                                         kInterface1b);
    if (err != ESP_OK) {return err;}

    return ESP_OK;
}

esp_err_t Max9867::setup_master_i2s() {
    esp_err_t err = i2s_new_channel(&reserved_i2s0::kChanCfg,
                                    &hI2sTx_, &hI2sRx_);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_init_std_mode(hI2sTx_, &reserved_i2s0::kStdCfg);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_init_std_mode(hI2sRx_, &reserved_i2s0::kStdCfg);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_enable(hI2sTx_);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_enable(hI2sRx_);
    if (err != ESP_OK) {return err;}


    return err;
}

esp_err_t Max9867::setup_adc_input() {
    constexpr uint8_t kMXINL_lineinOnly = (0b10 << 6);
    constexpr uint8_t kMXINR_noInput    = (0b00 << 4);
    constexpr uint8_t kSystemClk = (kMXINL_lineinOnly | kMXINR_noInput);
    constexpr uint8_t rSystemClk = 0x14;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rSystemClk, kSystemClk);
    return err;
}

esp_err_t Max9867::setup_audio_filter() {
    constexpr uint8_t kMODE_firAudio = (1 << 7);
    constexpr uint8_t kAVFLT_dcBlock = (1 << 4);
    constexpr uint8_t kDVFLT_dcBlock =  1;
    constexpr uint8_t kCodecFilter = kMODE_firAudio |
                                     kAVFLT_dcBlock |
                                     kDVFLT_dcBlock;
    constexpr uint8_t rCodecFilter= 0x0A;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rCodecFilter, kCodecFilter);
    return err;
}

void IRAM_ATTR Max9867::vTask(void *pvParameters) {
    Max9867* instance = static_cast<Max9867*>(pvParameters);
    size_t bytes_read, bytes_written;
    constexpr int kBufferSize = 1152;
    static int16_t buffer[kBufferSize]; // Pin buffer to RAM
    constexpr uint32_t kMaxBlockTime = 100;

    // int cycles = 50;
    // int gain = -39;
    for (;;) {
        // for (int i = 0; i < cycles; i++) {
            i2s_channel_read(instance->hI2sRx_, buffer, kBufferSize, &bytes_read, kMaxBlockTime);
            i2s_channel_write(instance->hI2sTx_, buffer, bytes_read, &bytes_written, kMaxBlockTime);
        // }
        // if (gain >= 0) {
        //     gain = -39;
        // }
        // else {
        //     gain++;
        // }
        // printf("Gain: %d\n", gain);
    }
}


    // uint8_t VOLLR;
    // switch (gain) {
    //     case -82: VOLLR = 0x26;
    //     case -78: VOLLR = 0x25;
    //     case -74: VOLLR = 0x24;
    //     case -70: VOLLR = 0x23;
    //     case -66: VOLLR = 0x22;
    //     case -62: VOLLR = 0x21;
    //     case -58: VOLLR = 0x20;
    //     case -54: VOLLR = 0x1F;
    //     case -50: VOLLR = 0x1E;
    //     case -46: VOLLR = 0x1D;
    //     case -42: VOLLR = 0x1C;
    //     case -38: VOLLR = 0x1B;
    //     case -34: VOLLR = 0x1A;
    //     case -30: VOLLR = 0x19;
    //     case -26: VOLLR = 0x18;
    //     case -22: VOLLR = 0x17;
    //     case -20: VOLLR = 0x16;
    //     case -18: VOLLR = 0x15;
    //     case -16: VOLLR = 0x14;
    //     case -14: VOLLR = 0x13;
    //     case -12: VOLLR = 0x12;
    //     case -10: VOLLR = 0x11;
    //     case  -8: VOLLR = 0x10;
    //     case  -6: VOLLR = 0x0F;
    //     case  -5: VOLLR = 0x0E;
    //     case  -4: VOLLR = 0x0D;
    //     case  -3: VOLLR = 0x0C;
    //     case  -2: VOLLR = 0x0B;
    //     case  -1: VOLLR = 0x0A;
    //     case   0: VOLLR = 0x09;
    //     case   1: VOLLR = 0x08;
    //     case   2: VOLLR = 0x07;
    //     case   3: VOLLR = 0x06;
    //     case   4: VOLLR = 0x04;
    //     case   5: VOLLR = 0x02;
    //     case   6: VOLLR = 0x00;
    // }