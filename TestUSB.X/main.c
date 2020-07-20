/*
 * File:   main.c
 * Author: Nelson Amador
 *
 * Created on July 18, 2020, 2:37 PM
 */
#define _XTAL_FREQ 48000000L
#include <xc.h>
#include <pic18f2550.h>
#include <string.h>
#include "global.h"

void main(void) {

    load_descriptor(&BUFFER, deviceDescriptor);
    while(1);
    
    return;
}
