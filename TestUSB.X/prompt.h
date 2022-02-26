/* 
 * File:   pront.h
 * Author: Nelson
 *
 * Created on February 25, 2022, 5:18 PM
 */

#ifndef PRONT_H
#define	PRONT_H

#ifdef	__cplusplus
extern "C" {
#endif

extern void commandLine(char *arg);
extern void processCommands(char* arg, void* cmd, void* sub);
extern void* getCommand();
extern void* getSubcommands();
extern char* getSubCommandValue(char command);

#ifdef	__cplusplus
}
#endif

#endif	/* PRONT_H */
