/* 
 * File:   pront.h
 * Author: Nelson
 *
 * Created on February 25, 2022, 5:18 PM
 */
#if __DEVTOOL__
#ifndef PRONT_H
#define	PRONT_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#define RESULT_LENGTH 16
extern void commandLine(char *arg, int size);
extern void processCommands(char* arg);
extern void* getCommand();
extern char* getSubCommandValue(char command);
extern char* getCommandKey();
extern int subCommandExists(char c);
void closeCommand();

#ifdef	__cplusplus
}
#endif

#endif	/* PRONT_H */
#endif
