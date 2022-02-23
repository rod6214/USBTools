#include <stdlib.h>
#include <xc.h>
#include "kernel.h"

volatile unsigned char BANK3[255] __at(0x300);

int mem_pointer = 0;
int mem_available = 500;

typedef struct _List 
{
    char data;
    void* next;
    void* prev;
}_List_t;

static void _incrementPointer();

List_t* CreateList() 
{
    List_t* newls = (List_t*)&BANK3[mem_pointer];
    _incrementPointer();
    return newls;
}

static void _incrementPointer() 
{
    int offset = sizeof(_List_t);
    mem_pointer += offset;
}

void kpush(List_t* ls, char data) 
{
    _List_t* old = (List_t*)ls;
    
    if (old->next)
    {
        _List_t* newls = (_List_t*)&BANK3[mem_pointer];
        newls->data = data;
        newls->prev = (void*)old->next;
        ((_List_t*)(old->next))->next = (void*)newls;
    }
    else 
    {
        old->data = data;
        old->next = (void*)old;
    }
    
    _incrementPointer();
}

int kcount(List_t* ls) 
{
    
    
    int count = 0;
    _List_t* old = (List_t*)ls;
    _List_t* pCurr = old;
    _List_t* pNext = (_List_t*)pCurr->next;
    
    if (ls == NULL || pNext == NULL) 
    {
        return 0;
    }
    
    if (pCurr == pNext) 
    {
        return 1;
    }
    
    while (pCurr != NULL) 
    {
        pCurr = (_List_t*)pCurr->next;
        count++;
    }

    return count;
}

void* knext(List_t* ls) 
{
    _List_t* old = (List_t*)ls;
    _List_t* pCurr = old;
    _List_t* pNext = (_List_t*)pCurr->next;
    
    if (ls == NULL || pNext == NULL) 
    {
        return NULL;
    }
    
    return pNext;
}

void* kprev(List_t* ls) 
{
    _List_t* old = (List_t*)ls;
    _List_t* pCurr = old;
    _List_t* pPrev = (_List_t*)pCurr->prev;
    
    if (ls == NULL) 
    {
        return NULL;
    }
    
    return pPrev;
}

void* kgetBegin(List_t* ls) 
{
    _List_t* old = (List_t*)ls;
    void* pCurr = ((_List_t*)(old->next))->next;
    return pCurr;
}

void* kgetLast(List_t* ls) 
{
    _List_t* old = (List_t*)ls;
    _List_t* pCurr = old;
    _List_t* pLast;
    
    while (pCurr != NULL) 
    {
        void* temp = (_List_t*)pCurr->next;
        if (temp == NULL) 
        {
            pLast = pCurr;
        }
        pCurr = (_List_t*)temp;
    }
    
    return pLast;
}

char kgetchar(List_t* ls) 
{
    _List_t* old = (List_t*)ls;
    return old->data;
}
