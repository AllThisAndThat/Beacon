#pragma once

#include "esp_err.h"

#include "driver/i2s_std.h"

/* Max notes
Page 26, 28
* TDM or I2S
* Either I2s 1 or 2 should work
* MSB always first
* Looks like philips mode on ESP -> WCL = 0, BCL = 0, DLY = 1, N_SDODLY = 0
*/

class I2s {
public:
    I2s();
    ~I2s();
    esp_err_t initiate();

private:
    i2s_chan_handle_t hChannel_;
    i2s_chan_handle_t hTxChannel_;
    i2s_chan_handle_t hRxChannel_;
};