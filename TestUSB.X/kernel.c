#include <stdlib.h>
#include <xc.h>
#include "kernel.h"

volatile unsigned char BANK2[255] __at(0x200);
volatile unsigned char BANK3[255] __at(0x300);

int mem_pointer = 0;
int mem_available = 500;
int stack_pointer = 0;

typedef struct _Allocation 
{
  void* next;
  void* ptr;
  void* link;
  int used;
  int length;
  int pos;
  int index;
} Allocation_t;

Allocation_t* kernel_alloc = NULL;
static void _incrementPointer(int bytes);
static int _alloc_count(Allocation_t* alloc);
static void _alloc_push(List_t* ls, char data);
static char _alloc_pop(List_t* ls);

List_t* CreateList(int bytes) 
{
    if (kernel_alloc == NULL) 
    {
        kernel_alloc = (Allocation_t*)&BANK2[mem_pointer];
    }
    List_t* ptr = kmalloc(bytes);
    return ptr;
}

char alloc_getData(List_t* ls, int index) 
{
    Allocation_t* alloc = (Allocation_t*)ls;
    int lastLength = alloc->length;

    while(alloc != NULL) 
    {
        int pos = index;
        int overflow = pos >= lastLength;
        
        if (!overflow)
        {
            int idx = alloc->length == lastLength ? pos : abs(lastLength - alloc->length - pos);
            unsigned char* ptr = alloc->ptr;
            char data = ptr[idx];
            return data;
        }

        alloc = alloc->link;
        lastLength += alloc->length;
    }

    return (char)-1;
}

static void _alloc_push(List_t* ls, char data) 
{
    Allocation_t* alloc = (Allocation_t*)ls;
    Allocation_t* principal = (Allocation_t*)ls;
    int lastLength = alloc->length;

    while(alloc != NULL) 
    {
        int pos = principal->pos;
        int overflow = pos >= lastLength;
        
        if (!overflow) 
        {
            unsigned char* ptr = alloc->ptr;
            int idx = alloc->length == lastLength ? pos : abs(lastLength - alloc->length - pos);
            ptr[idx] = data;
            principal->pos++;
            return;
        }
        alloc = alloc->link;
        lastLength += alloc->length;
    }
}

static char _alloc_pop(List_t* ls) 
{
    Allocation_t* alloc = ls;
    Allocation_t* principal = ls;
    int lastLength = alloc->length;

    while(alloc != NULL) 
    {
        int pos = principal->pos;
        int overflow = pos >= lastLength;
        if (!overflow)
        {
            int idx = alloc->length == lastLength ? pos : abs(lastLength - alloc->length - pos);
            unsigned char* ptr = alloc->ptr;
            char data = ptr[idx - 1];
            ptr[idx - 1] = '\0';
            principal->pos--;
            return data;
        }
        alloc = alloc->link;
        lastLength += alloc->length;
    }

    return -1;
}


void kpush(List_t* ls, char data) 
{
    _alloc_push(ls, data);
}

char kpop(List_t* ls)
{
    char c = _alloc_pop(ls);
    return c;
}

int kcount(List_t* ls) 
{
    Allocation_t* alloc = (Allocation_t*)ls;
    return alloc->pos;
}

void* knext(List_t* ls) 
{
    Allocation_t* alloc = ls;
    alloc->index++;
    return ls;
}

void* kprev(List_t* ls) 
{    
    return NULL;
}

void* kgetBegin(List_t* ls) 
{
    return NULL;
}

void* kgetLast(List_t* ls) 
{
    return NULL;
}

char kgetchar(List_t* ls) 
{
    Allocation_t* alloc = ls;
    int index = alloc->index;
    char data = alloc_getData(ls, index);
    return data;
}

static int _alloc_count(Allocation_t* alloc) 
{
    int count = 0;
    Allocation_t* pCurr = alloc;
    
    if (pCurr->next == NULL || alloc == NULL) 
    {
        return NULL;
    }
    else if (pCurr == pCurr->next) 
    {
        return 1;
    }
    
    while(pCurr != NULL) 
    {
        pCurr = (Allocation_t*)pCurr->next;
        count++;
    }
    
    return count;
}

static void _incrementPointer(int bytes) 
{
    volatile int size = sizeof(Allocation_t);
    mem_pointer += size;
    stack_pointer += bytes;
}

void* kmalloc(int bytes)
{
    
    int count = _alloc_count(kernel_alloc);
    Allocation_t* alloc = NULL;
    int requiredLength = bytes;
    
    if (count > 0) 
    {
        Allocation_t* pCurr = kernel_alloc;
        int finish = FALSE;
        
        int same = pCurr == pCurr->next;
        
        while(pCurr != NULL && !finish && !same)
        { 
            if (!pCurr->used) 
            {
                if (alloc != NULL) 
                {
                    Allocation_t* linked = alloc->link;
                    linked->link = pCurr;
                    linked->used = TRUE;
                }
                else
                {
                    alloc = pCurr;
                    alloc->link = pCurr;
                    alloc->used = TRUE;
                }

                requiredLength -= pCurr->length;
                finish = (bytes - pCurr->length) <= 0;
            }

            pCurr = pCurr->next;
        }
    }

    if (requiredLength > 0) 
    {
        Allocation_t* pCurr = kernel_alloc;
        Allocation_t* pLast;
        
        if (pCurr->next) 
        {
            Allocation_t* newAlloc = (Allocation_t*)&BANK2[mem_pointer];
            Allocation_t* curr = pCurr->next;
            pLast = newAlloc;
            curr->next = newAlloc;
            newAlloc->length = requiredLength;
            newAlloc->used = TRUE;
            newAlloc->ptr = (void*)&BANK3[stack_pointer];
            newAlloc->pos = 0;
            newAlloc->index = 0;
        }
        else 
        {
            pLast = pCurr;
            pCurr->next = pCurr;
            pCurr->length = requiredLength;
            pCurr->used = TRUE;
            pCurr->ptr = (void*)&BANK3[stack_pointer];
            pCurr->pos = 0; 
            pCurr->index = 0;
        }

        if (alloc != NULL) 
        {
            alloc->link = pLast;
        }
        else 
        {
            alloc = pLast;
        }

        _incrementPointer(requiredLength);
    }

    return alloc;
}

void kfree(void* ptr) 
{
    Allocation_t* alloc = ptr;
    
    if (alloc != NULL) 
    {
        alloc->used = FALSE;
        alloc->link = NULL;
        alloc->pos = 0;
    }
}
