#include <stdlib.h>
#include <xc.h>
#include "kernel.h"

volatile unsigned char BANK2[255] __at(0x200);
volatile unsigned char BANK3[255] __at(0x300);

int mem_pointer = 0;
int mem_available = 500;
int stack_pointer = 0;

typedef struct _List 
{
    char data;
    void* next;
    void* prev;
}_List_t;

typedef struct _Allocation 
{
  void* next;
  void* ptr;
  void* link;
  int used;
  int length;
  int pos;
} Allocation_t;



Allocation_t* kernel_alloc = NULL;
static void _incrementPointer(int bytes);
static int _alloc_count(Allocation_t* alloc);

List_t* CreateList(int bytes) 
{
    if (kernel_alloc == NULL) 
    {
        kernel_alloc = (Allocation_t*)&BANK2[mem_pointer];
    }
    List_t* ptr = kmalloc(bytes);
    return ptr;
}


void kpush(List_t* ls, char data) 
{
    Allocation_t* alloc = ls;

    while(alloc != NULL) 
    {
        int pos = alloc->pos;
        if (pos < alloc->length) 
        {
            unsigned char* ptr = alloc->ptr;
            ptr[pos] = data;
            alloc->pos++;
            return;
        }
        alloc = alloc->link;
    }
}

char kpop(List_t* ls)
{
    Allocation_t* alloc = ls;

    while(alloc != NULL) 
    {
        int pos = alloc->pos;
        int overflow = pos < alloc->length && alloc->link != NULL;
        if (!overflow)
        {
            unsigned char* ptr = alloc->ptr;
            char data = ptr[pos - 1];
            ptr[pos - 1] = '\0';
            alloc->pos--;
            return data;
        }
        alloc = alloc->link;
    }

    return -1;
}

int kcount(List_t* ls) 
{
    int count = _alloc_count(ls);
    return count;
}

void* knext(List_t* ls) 
{
    Allocation_t* alloc = ls;
    return alloc->link;
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
    return 0;
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
        }
        else 
        {
            pLast = pCurr;
            pCurr->next = pCurr;
            pCurr->length = requiredLength;
            pCurr->used = TRUE;
            pCurr->ptr = (void*)&BANK3[stack_pointer];
            pCurr->pos = 0; 
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
