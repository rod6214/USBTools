/* 
 * File:   logger_test.h
 * Author: Nelson
 *
 * Created on March 4, 2022, 5:37 PM
 */
#if __LOGGER_TEST__
#ifndef LOGGER_TEST_H
#define	LOGGER_TEST_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../cstream.h"
#include "../logger.h"
#include "../kernel.h"
int Logger_ExecuteTests();

#ifdef	__cplusplus
}
#endif

#endif	/* LOGGER_TEST_H */
#endif
