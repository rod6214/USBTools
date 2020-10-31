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
#define SDA_BIT (HIGH_SDA - 1)

#define set_clock_low(port) (*port) &= LOW_SCLK
#define set_clock_high(port) (*port) |= HIGH_SCLK
#define set_sda_low(port) (*port) &= LOW_SDA
#define set_sda_high(port) (*port) |= HIGH_SDA
#define bit_test(data, bit) ((data & (1 << bit)) == (1 << bit))
#define set_sda_out(tris) (*tris) &= LOW_SDA
#define set_sda_in(tris) (*tris) |= HIGH_SDA
#define bit_set(ptr, bit) (*ptr) |= (1 << bit) 
#define bit_clear(ptr, bit) (*ptr) &= ~(1 << bit)
#define test_sda(port) bit_test((*port), SDA_BIT)
#define set_index(data, i) (*data) = i
#define wait_ack(port) while(!bit_test(*port, SDA_BIT))
#define wait_noack(port) while(bit_test(*port, SDA_BIT))
#define is_random_read(data) data == 1
#define set_ack(port) set_sda_low(port)

static void __process_data(I2C_t *i2c_handle, BYTE mode, BYTE index);
static BYTE __get_data(I2C_t *i2c_handle);
static void __bit_shift(I2C_t *i2c_handle, BYTE data, BYTE mode, BYTE random);

static void __bit_shift(I2C_t *i2c_handle, BYTE data, BYTE mode, BYTE random) {
    *(i2c_handle->data) = data;
    for(int i = 0; i < 10; i++) {
        if (i < 9) {
            for (int state = 0; state < 3; state++) {
                switch (state)
                {
                case 0:
                    set_clock_low(i2c_handle->port);
                    break;
                case 1:
                    __process_data(i2c_handle, mode, i);
                    break;
                case 2:
                    set_clock_high(i2c_handle->port);
                    break;
                }
                __delay_ms(TIME_MS);
            }
        } else {
            for (int state = 0; state < 2; state++) {
                if (state == 0) {
                    set_clock_low(i2c_handle->port);
                    switch (mode)
                    {
                    case I2C_WRITE:
                        {
                            set_sda_in(i2c_handle->tris);
                            wait_ack(i2c_handle->port);
                        }
                        break;
                    case I2C_READ:
                        {
                            if (is_random_read(random)) {
                                set_ack(i2c_handle->port);
                            } else {
                                set_sda_in(i2c_handle->tris);
                                wait_noack(i2c_handle->port);
                            }
                        }
                        break;
                    }
                } else {
                    set_sda_out(i2c_handle->tris);
                    set_sda_low(i2c_handle->port);
                }
                __delay_ms(TIME_MS);
            }
        }
    }
}

// void i2c_write() {

// }

// void i2c_read(I2C_t *i2c_handle, BYTE *buffer, int bytes) {

// }

void i2c_init(I2C_t *i2c_handle) {
    static BYTE data;
    i2c_handle->data = &data;
}

static void __process_data(I2C_t *i2c_handle, BYTE mode, BYTE index) {
    switch (mode)
    {
    case I2C_WRITE:
        {
            if ((*(i2c_handle->data) & 128) == 128) {
                if (index < 7) {
                    set_sda_high(i2c_handle->port);
                }
                else {
                    set_sda_in(i2c_handle->tris);
                }
            } else {
                set_sda_low(i2c_handle->port);
            }
            *(i2c_handle->data) <<= 1;
        }
        break;
    case I2C_READ:
        {
            set_sda_in(i2c_handle->tris);
            
            if (test_sda(i2c_handle->port)) {
                bit_set(i2c_handle->data, 0);
            } else {
                bit_clear(i2c_handle->data, 0);
            }

            if (index < 7) {
                *(i2c_handle->data) <<= 1;
            }
        }
        break;
    }
}

static BYTE __get_data(I2C_t *i2c_handle) {
    return *(i2c_handle->data);
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

void receive_serial(I2C_t *i2c_handle, BYTE *data, int bytes) {
    data[0] = 0xA0;
    data[1] = 0;
    data[2] = 0;

    send_serial(i2c_handle, data, 3);
    start_serial(i2c_handle);
    __bit_shift(i2c_handle, 0xA1, I2C_WRITE, FALSE);
    
    if (bytes > 1) {
        for (int i = 0; i < bytes; i++) {
            __bit_shift(i2c_handle, 0, I2C_READ, FALSE);
            data[i] = __get_data(i2c_handle);
        }
    } else {
        __bit_shift(i2c_handle, 0, I2C_READ, TRUE);
        data[0] = __get_data(i2c_handle);
    }
    
    stop_serial(i2c_handle);
}

void send_serial(I2C_t *i2c_handle, BYTE *data, int bytes) {
    start_serial(i2c_handle);
    for (int i = 0; i < bytes; i++) {
        __bit_shift(i2c_handle, data[i], I2C_WRITE, FALSE);
    }
    stop_serial(i2c_handle);
}
