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
#include "descriptors.h"
//volatile BDnSTAT_t BD1STAT __at(BDMEM(1));

// extern const unsigned char deviceDescriptor[];
extern const DeviceDescriptor_t deviceDescriptor;
extern const struct HIDConfigurationDescriptior_t configurationDesc;
extern const BYTE stringDescriptor[];
extern const BYTE iProductDesc[];
extern const BYTE iSerialNumberDesc[];

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

