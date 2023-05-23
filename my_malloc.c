#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

struct malloc_stc
{
    struct malloc_stc *next;
    int size;
    unsigned char *buffer; // points to the first byte of storage in the buffer usable by the caller of MyMalloc()
};

static struct malloc_stc *MyFreeList = NULL;
static struct malloc_stc *MyAllocatedList = NULL;
// unsigned char represents a byte of data
static unsigned char MyBuff[MAX_MALLOC_SIZE];

void InitMyMalloc()
{
    MyFreeList = (struct malloc_stc *)MyBuff;
    MyFreeList->next = NULL;
    MyFreeList->size = sizeof(MyBuff) - sizeof(struct malloc_stc);
    MyFreeList->buffer = (unsigned char *)(MyBuff + sizeof(struct malloc_stc));
    MyAllocatedList = NULL;
}

void *MyMalloc(int size)
{
    struct malloc_stc *current = MyFreeList;
    struct malloc_stc *prev = NULL;

    while (current != NULL)
    {
        if (current->size >= size)
        {
            if (current->size - size > sizeof(struct malloc_stc))
            {
                struct malloc_stc *newBlock = (struct malloc_stc *)(current->buffer + size);
                newBlock->next = current->next;
                newBlock->size = current->size - size - sizeof(struct malloc_stc);
                newBlock->buffer = current->buffer + size + sizeof(struct malloc_stc);
                if (current == MyFreeList)
                    MyFreeList = newBlock;
                else
                    prev->next = newBlock;
            }
            else
            {
                if (current == MyFreeList)
                    MyFreeList = current->next;
                else
                    prev->next = current->next;
            }

            current->next = MyAllocatedList;
            MyAllocatedList = current;

            current->size = size;

            return (void *)current->buffer;
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void MyFree(void *buffer)
{
    if (buffer != NULL)
    {
        struct malloc_stc *current = MyAllocatedList;
        struct malloc_stc *prev = NULL;

        while (current != NULL && current->buffer != buffer)
        {
            prev = current;
            current = current->next;
        }

        if (current != NULL)
        {
            if (prev == NULL)
                MyAllocatedList = current->next;
            else
                prev->next = current->next;

            current->next = MyFreeList;
            MyFreeList = current;
        }
    }
}

void PrintMyMallocFreeList()
{
    struct malloc_stc *current = MyFreeList;

    printf("Free List:\n");
    while (current != NULL)
    {
        printf("block: %p\n", current);
        printf("\tsize: %d\n", current->size);
        printf("\tnext: %p\n", current->next);
        printf("\tbuffer: %p\n", current->buffer);
        current = current->next;
    }
}
