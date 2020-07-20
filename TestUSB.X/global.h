/* 
 * File:   global.h
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 12:35 AM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <usb.h>

volatile BDnSTAT_t BD1STAT __at(BDMEM(1));
volatile DeviceDescriptor_t BUFFER __at(0x500);

extern const unsigned char deviceDescriptor[];

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

