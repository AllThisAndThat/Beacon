#pragma once

#include "esp_err.h"
#include "driver/i2s_std.h"

#include "i2c.h"

/*
Max9867 Terminology:
For single ended inputs only (there are also microphone inputs)
- Playback - path from input to headphone amplifier
- Record - sampled by ADC
- DC Tracking - can you ADC to sample DC voltages
- Capacitorless ouptut - might have the advantage of more power
  delivery and higher impedance headphones
- Left/right preamp - amplifier for single ended inputs
- Input Mix - can mix single-ended inputs and microphone inputs
- Output Mix - mix both DACs and playback signals
- Low pass and high pass filtering available
- Sidetone - mix record and playback signals after digital filtering (DAC)
- AUX ADC - adc designed to sample dc voltage. Can also sample audio same time.
- NI value can be used so you don't need to provide LRCLK or BCLK 
- Should calibrate for DC measurements

List of I2C tasks:
*0x00: Status
    clip detect - input at max adc value
    slew level detect - volume gradually changes. Flag when final value reached
    pll unlocked - signal is not reliable
*0x01: Jack sense
    LSNS  - no load on LOUTP
    jksns - no jack on jacksns - don't need
    jkmic - no microphone detected - always plugged in
0x02/0x03: AUX
    16 bit 2's complement voltage on Jacksns/Aux
0x04: Interrupt enable
*0x05: System Clock
    MCLK prescaler -> depends on MCLK value, generates PCLK
    Freq -> set to 0x00 for normal PLL mode
*0x06/0x07: NI
    PLL -> set to 0x0 sets LRCLK based off NI value
    NI -> using a table, set to 0x6000 for 48kHz given MCLK of 12.288MHz
*0x08: Interface Mode
    MAS -> 0 set to slave mode
    WCI -> 0 device comm is active when LRCLK is low
    BCI -> 0 , data is latched on rising/falling edge (0 = rising, 1 = falling)
    SDODLY -> 0, when SDOUT is sent (same as SDIN, or half cycle later)
    DLY -> 1, Sets to I2S philips standard, latch MSB one cycle after LRCLK
    HIZOFF -> 1, no high z mode, not shared with other devices
    TDM -> 0, no TDM mode
*0x09: Interface mode
    DMONO -> 1, audio from input is routed to both DACs
    BSEL -> X, don't care since we are in slave mode
0x0A: Codec Filters
    Mode: -> ?,IIR voice or FIR audio filter
    AVFLT: ?
    DVFLT: ?
0x0B: Sidetones (pg 35)
    DSTS - can mix L, R, or L+R with playback
    DVST - can set gain of headphone output or single-ended headphone output
0x0C: DAC Level
    DACM -> 0 = no mute- can mute output
    DACG -> 00 = no gain, dac gain
    DACA -> 0x3 = 0dB, fine gain control
0x0D: ADC Level
    AVL -> 0x03 = 0dB, adc left gain
    AVR -> 0x03, adc right gain
0x0E/0F: Line input
    LILM: can turn on/off direct output playback mode
    LIRM: right line input is connected to the headphone amplifier
    Set gain of playback path
0x10/0x11: 
    Mute playback path
    control playback gain
0x12/0x13:
    Control microphone circuitry
0x14: ADC input
    MXINL/R
    Set which inputs and what's mixed - 10 = left and right single ended.
    It doesn't look like you can use microhphones at the same time.
    AUXCAP
    0 = Hold AUX for jacksns. Don't want AUX
    AUXEN = 0, use aux for jack detection only.
0x15: Microphone mix
    Can disable microphones
0x16: Mode
    DSLEW - sets how fast volume changes
    VSEM - enable/disable volume slew
    ZDEN - change volume at zero crossing or immediately
    JDETEN - turns on jack detection
    HPMODE - headphone mode, depends on what's connected Headphones or speaker
0x17: system shutdown
    N_SHDN - used for reset?
0xFF: Revision

Setup Steps:
1. Set clocking mode
    - Slave with LRCLK and BCLK provided - sets the sample rates.
2. Disable JACKSNS
3. Enable Left and right ADC.
4. Remove reset hold

* set system shutdown to enable ic
*/

enum class JackState : uint8_t {
    kPlugged   = 0,
    kUnplugged = 1
};

class Max9867 {
public:
    Max9867();
    ~Max9867();
    esp_err_t initiate();

    esp_err_t get_hp_jack_state(JackState& state);

    esp_err_t action_probe();
    esp_err_t action_verify();

    esp_err_t test_read_audio();
    esp_err_t test_echo_audio();
    esp_err_t test_write_audio();

    i2s_chan_handle_t hI2sTx_; // TODO: move this to private
    i2s_chan_handle_t hI2sRx_; 

private:
    I2c i2c_;
    i2c_master_dev_handle_t hDevice_;

    esp_err_t setup_i2c();
    esp_err_t setup_mode();
    esp_err_t setup_system_clock();
    esp_err_t setup_interface_mode();
    esp_err_t setup_master_i2s();

    esp_err_t test_etc_setup();
    
    // esp_err_t setup_codec_filters();
    // esp_err_t setup_dac_gain();
    // esp_err_t setup_adc_gain();
    // esp_err_t setup_input_signals();
    // esp_err_t setup_output_signals();

    // esp_err_t action_shutdown();

};

/* Multimeter can only measure to 50 kHz
Pin  7 -> MCLK ->  12.288 MHz
Pin 15 -> BCLK ->  769 kHz
Pin 16 -> LRCLK -> 48 kHz

MCLK = 1.528V
BCLK = 1.65V
LRCLK = 1.65V
DOUT = 



*/