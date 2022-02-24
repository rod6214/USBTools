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
} Allocation_t;



Allocation_t kernel_alloc;
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
    _List_t* old = ls;
    
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
    _List_t* old = ls;
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
    _List_t* old = ls;
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

void* kmalloc(int bytes)
{
    
    int count = _alloc_count(&kernel_alloc);
    Allocation_t* alloc = NULL;
    int requiredLength = bytes;
    
    if (count > 0) 
    {
        Allocation_t* pCurr = &kernel_alloc;
        int finish = FALSE;
        
        int same = pCurr == pCurr->next;
        
        while(pCurr != NULL && !finish && !same) 
//        while(pCurr != NULL && !same) 
        {
//            int le = pCurr->length;
//            PORTB = le;
            
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
        Allocation_t* pCurr = &kernel_alloc;
        
        if (pCurr->next) 
        {
            Allocation_t* newAlloc = (Allocation_t*)&BANK2[mem_pointer];
            mem_pointer += sizeof(Allocation_t);
            Allocation_t* curr = pCurr->next;
            curr->next = newAlloc;
            newAlloc->length = requiredLength;
            newAlloc->used = TRUE;
            newAlloc->ptr = (void*)&BANK3[stack_pointer];
        }
        else 
        {
            pCurr->next = pCurr;
            pCurr->length = requiredLength;
            pCurr->used = TRUE;
            pCurr->ptr = (void*)&BANK3[stack_pointer];
        }

        if (alloc != NULL) 
        {
            Allocation_t* curr = pCurr->next;
            alloc->link = curr->next;
        }
        else 
        {
            Allocation_t* curr = pCurr->next;
            alloc = curr;
        }

        stack_pointer += requiredLength;
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
    }
}
