#include <p18cxxx.h>
#include "i2c.h"

#define _XTAL_FREQ 4000000

#define START           0
#define BYTE_CONTROL    1
#define ACK             2
#define ADDRESS_HIGH    3
#define ADDRESS_LOW     4
#define DATA            5
#define STOP            6

#define LOW_SCLK ~2
#define LOW_SDA ~1
#define HIGH_SCLK 2
#define HIGH_SDA 1
#define TIME_MS 1

#define set_clock_low(port) (*port) &= LOW_SCLK
#define set_clock_high(port) (*port) |= HIGH_SCLK
#define set_sda_low(port) (*port) &= LOW_SDA
#define set_sda_high(port) (*port) |= HIGH_SDA
#define bit_test(data, bit) ((data & (1 << bit)) == (1 << bit))
#define set_sda_out(tris) (*tris) &= LOW_SDA
#define set_sda_in(tris) (*tris) |= HIGH_SDA

void bit_shift(I2C_t *i2c_handle, BYTE data, BYTE last) { 
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            set_clock_low(i2c_handle->port);
        }  else if (i == 1) {
            if (bit_test(data, 7)) {
                if (last) {
                    set_sda_in(i2c_handle->tris);
                } else {
                    set_sda_high(i2c_handle->port);
                }
            } else {
                set_sda_low(i2c_handle->port);
            }
        } else if (i == 2) {
            set_clock_high(i2c_handle->port);
        }
        __delay_ms(TIME_MS);
    }
}

void i2c_write() {

}

void start_serial(I2C_t *i2c_handle) {
    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            set_sda_low(i2c_handle->port);
        }
        __delay_ms(TIME_MS);
    }
}

void stop_serial(I2C_t *i2c_handle) {
    for (int i = 0; i < 4; i++) {
        if (i == 0) {
            set_clock_low(i2c_handle->port);
        } else if (i == 1) {
            set_sda_low(i2c_handle->port);
        } else if (i == 2) {
            set_clock_high(i2c_handle->port);
        } else if (i == 3) {
            set_sda_high(i2c_handle->port);
        }
        __delay_ms(TIME_MS);
    }
}

void wait_serial(I2C_t *i2c_handle) {
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            set_sda_in(i2c_handle->tris);
            set_clock_low(i2c_handle->port);
            while (!bit_test(*(i2c_handle->port), 0))
            ;
        }  else if (i == 1) {
            set_sda_out(i2c_handle->tris);
            set_sda_low(i2c_handle->port);
        }
        __delay_ms(TIME_MS);
    }
}

void send_serial(I2C_t *i2c_handle, BYTE *data, int bytes) {
    start_serial(i2c_handle);
    for (int i = 0; i < bytes; i++) {
        BYTE data_temp = data[i];
        for (int j = 0; j < 10; j++) {
            if (j < 9) {
                bit_shift(i2c_handle, data_temp, j >= 7);
                data_temp = data_temp << 1;
            } else {
                wait_serial(i2c_handle);
            }
        }
    }
    stop_serial(i2c_handle);
}
