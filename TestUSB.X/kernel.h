/* 
 * File:   kernel.h
 * Author: Nelson
 *
 * Created on February 22, 2022, 4:29 PM
 */

#ifndef KERNEL_H
#define	KERNEL_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct List 
{
    int length;
} List_t;

#define keval(x) (x != NULL)
#define kinc(x) (x = knext((List_t*)x))
#define foreach(y) int count = kcount(y); \
for(int i = 0; y != NULL && i < count; kinc(y), i++)

extern List_t* CreateList();
extern void kpush(List_t* ls, char data);
extern int kcount(List_t* ls);
extern void* kgetBegin(List_t* ls);
extern void* kgetLast(List_t* ls);
extern void* knext(List_t* ls);
extern char kgetchar(List_t* ls);
extern void* kprev(List_t* ls);
#ifdef	__cplusplus
}
#endif

#endif	/* KERNEL_H */

