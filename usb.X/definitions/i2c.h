#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "datatypes.h"

void send_serial(int* port, BYTE* data, int bits);

#ifdef	__cplusplus
}
#endif

#endif	/* USB_H */
