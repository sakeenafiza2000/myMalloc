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

        // Find the allocated block associated with the buffer pointer.
        while (current != NULL && current->buffer != buffer)
        {
            prev = current;
            current = current->next;
        }

        if (current != NULL)
        {
            // Remove the block from the allocated list.
            if (prev == NULL)
                MyAllocatedList = current->next;
            else
                prev->next = current->next;

            // Add the block to the free list.
            current->next = MyFreeList;
            MyFreeList = current;

            // Perform coalescing if possible.
            struct malloc_stc *mergedBlock = MyFreeList;
            struct malloc_stc *mergedPrev = NULL;

            while (mergedBlock != NULL)
            {
                struct malloc_stc *nextBlock = mergedBlock->next;

                // Check if the current block and the next block in the free list are adjacent.
                if (nextBlock != NULL && mergedBlock->buffer + mergedBlock->size + sizeof(struct malloc_stc) == nextBlock->buffer)
                {
                    // Merge the two blocks by updating the size and buffer of the current block.
                    mergedBlock->size += nextBlock->size + sizeof(struct malloc_stc);
                    mergedBlock->next = nextBlock->next;

                    // Remove the next block from the free list.
                    if (mergedPrev == NULL)
                        MyFreeList = mergedBlock;
                    else
                        mergedPrev->next = mergedBlock;
                }
                else
                {
                    mergedPrev = mergedBlock;
                    mergedBlock = nextBlock;
                }
            }
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
