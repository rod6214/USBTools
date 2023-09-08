/* 
 * File:   printer.h
 * Author: Nelson
 *
 * Created on February 21, 2022, 8:37 PM
 */
#if __LOGGER__
#ifndef PRINTER_H
#define	PRINTER_H

#include "cstream.h"

#ifdef	__cplusplus
extern "C" {
#endif

extern const unsigned char* message_list[];
extern void* CreateLogger(int bytes);
extern int log_putchar(char c);
extern char log_getchar();
extern void log_rewind();
extern void log_free();

#ifdef	__cplusplus
}
#endif

#endif	/* PRINTER_H */
#endif