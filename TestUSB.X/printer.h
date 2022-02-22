/* 
 * File:   printer.h
 * Author: Nelson
 *
 * Created on February 21, 2022, 8:37 PM
 */

#ifndef PRINTER_H
#define	PRINTER_H

#include "cstream.h"

#ifdef	__cplusplus
extern "C" {
#endif

extern const unsigned char* message_list[];
extern STREAM CreatePrinter();
extern void InitDevTool(STREAM stream);


#ifdef	__cplusplus
}
#endif

#endif	/* PRINTER_H */

