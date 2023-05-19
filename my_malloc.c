#include "my_malloc.h"

#include <stdio.h> 
#include <stdlib.h> 

struct malloc_stc
{
    struct malloc_stc *next;
    struct malloc_stc *prev; 
    int size;
    unsigned char *buffer; //points to the first byte of storage in the buffer usable by the caller of MyMalloc()
};

static struct malloc_stc *MyFreeList; 
//unsigned char represents a bye of data
static unsigned char MyBuff[MAX_MALLOC_SIZE];

//initialize the buffer so there is one big block containing all of the free storage you can allocate
//initialize the free list head pointer to point to this block
void InitMyMalloc()
{
    //assigns the address of the MyBuff array to the MyFreeList pointer. 
    //It casts the address to the type struct malloc_stc* to ensure proper pointer arithmetic.
    MyFreeList = (struct malloc_stc *)MyBuff;
    MyFreeList->next = NULL;
    MyFreeList->prev = NULL;
    //It calculates the size of the free storage available in the buffer. 
    MyFreeList->size = sizeof(MyBuff) - sizeof(struct malloc_stc);
    //It assigns the address of the first byte of usable storage in the buffer to the buffer pointer of the MyFreeList node.
    //It calculates this address by adding the size of the struct malloc_stc structure to the address of MyBuff.
    //we effectively move the pointer forward by sizeof(struct malloc_stc) bytes.
    MyFreeList->buffer = (unsigned char *)(MyBuff + sizeof(struct malloc_stc));

}

void PrintMyMallocFreeList()
{
    struct malloc_stc *current = MyFreeList;

    printf("Free List:\n");
    while (current != NULL)
    {
        printf("Node: %p, Size: %d\n", current, current->size);
        current = current->next;
    }
}