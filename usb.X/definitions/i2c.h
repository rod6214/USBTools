#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <p18cxxx.h>
#include "datatypes.h"

typedef struct _I2C_t {
    int *port;
    int *tris;
} I2C_t;

extern void send_serial(I2C_t *i2c_handle, BYTE data, int bits);
extern void wait_serial(I2C_t *i2c_handle);
extern void start_serial(I2C_t *i2c_handle);
extern void stop_serial(I2C_t *i2c_handle);

#ifdef	__cplusplus
}
#endif

#endif	/* USB_H */
