/*
 * File:   loaders.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 6:52 PM
 */


#include <xc.h>
#include <string.h>
#include "usb.h"

void load_descriptor(void* dest, void* srcDesc) {
    unsigned char len = ((unsigned char*)srcDesc)[0];
    if (dest) {
        memcpy(dest, srcDesc, len);
    }
}

//USBRequest* get_request() {
//    return 
//}
