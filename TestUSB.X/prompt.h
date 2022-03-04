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
extern char getSubCommandKey(int index);
extern char* getCommandKey();
extern int subCommandExists(char c);
void closeCommand();

const char* message_list[] = 
{
    "DevTool for PIC18, your free communication program\n",
    "version 0.0.0.1\n",
    "You can start by typing a command:\n",
    "Listening...\n",
    "Are you sure to quick?",
};

#ifdef	__cplusplus
}
#endif

#endif	/* PRONT_H */
