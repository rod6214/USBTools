#include <p18cxxx.h>
#include "i2c.h"

#define START           0
#define BYTE_CONTROL    1
#define ACK             2
#define ADDRESS_HIGH    3
#define ADDRESS_LOW     4
#define DATA            5
#define STOP            6

void start_serial(I2C_t *i2c_handle) {
    (*(i2c_handle->port)) |= 3;
    (*(i2c_handle->tris)) &= ~3;
    for(int i = 0; i < 2; i++);
    (*(i2c_handle->port)) &= ~2;
}

void stop_serial(I2C_t *i2c_handle) {
    for(int i = 0; i < 4; i++) {
        if (i == 0) {
            (*(i2c_handle->port)) &= ~2;
        } else if (i == 1) {
            (*(i2c_handle->port)) &= ~1;
        } else if (i == 2) {
            (*(i2c_handle->port)) |= 1;
        } else if (i == 3) {
            (*(i2c_handle->port)) |= 2;
        }
    }
}

void wait_serial(I2C_t *i2c_handle) {
    (*(i2c_handle->tris)) |= 2;
    (*(i2c_handle->port)) &= ~1;
    while((((*(i2c_handle->port)) & 2) != 2));
    (*(i2c_handle->tris)) &= ~2;
}

void send_serial(I2C_t *i2c_handle, BYTE data, int bytes) {
    int bits = 32;
    BYTE dataTemp = data;
    (*(i2c_handle->tris)) &= ~3;

    for (int i = 0, j = 1; i < bits; i++, j++) {

        if (j == 1) {
            if ((*(i2c_handle->port) & 1) == 1) {
                (*(i2c_handle->port)) &= ~1;
            }
            else {
                (*(i2c_handle->port)) |= 1;
            }
        } 
        else if (j == 2) {
            if (((*(i2c_handle->port)) & 1) == 0) {

                if (dataTemp & 128) {
                    (*(i2c_handle->port)) |= 2;
                } else {
                    (*(i2c_handle->port)) &= ~2;
                }

                dataTemp = dataTemp << 1;
            }
            j = 0;
        }
    }
}
