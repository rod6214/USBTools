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

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define keval(x) (x != NULL)
#define kinc(x) (x = knext((List_t*)x))
#define foreach(y) int count = kcount(y); \
for(int i = 0; y != NULL && i < count; kinc(y), i++)

extern void* CreateList(int bytes);
extern void kpush(void* ls, char data);
extern int kcount(void* ls);
extern void* kgetBegin(void* ls);
extern void* kgetLast(void* ls);
extern void* knext(void* ls);
extern char kgetchar(void* ls);
extern void* kprev(void* ls);
extern void* kmalloc(int bytes);
void kfree(void* ptr);
char kpop(void* ls);
char alloc_getData(void* ls, int index);

#ifdef	__cplusplus
}
#endif

#endif	/* KERNEL_H */

