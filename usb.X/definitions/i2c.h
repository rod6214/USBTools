#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <p18cxxx.h>
#include "datatypes.h"

#define I2C_WRITE 0
#define I2C_READ 1
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
    
typedef struct _I2C_t {
    int *port;
    int *tris;
    BYTE *data;
} I2C_t;

extern void send_serial(I2C_t *i2c_handle, BYTE *data, int bytes);
extern void receive_serial(I2C_t *i2c_handle, BYTE *data, int bytes);
extern void start_serial(I2C_t *i2c_handle);
extern void stop_serial(I2C_t *i2c_handle);

#ifdef	__cplusplus
}
#endif

#endif	/* USB_H */
