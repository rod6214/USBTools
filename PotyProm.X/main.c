/*
 * File:   main.c
 * Author: Nelson
 *
 * Created on April 24, 2022, 12:22 PM
 */

#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cstream.h"
#include "prompt.h"

void config();
void loop();

void main(void) {
    config();
    while(1) {
        loop();
    }
    return;
}

void config(){
    usb_SetUsbAsHighPriority();
    usb_init();
}

void loop(){}
