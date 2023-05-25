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


//initialize the buffer so there is one big block containing all of the free storage you can allocate
//initialize the free list head pointer to point to this block
void InitMyMalloc()
{
    //assigns the address of the MyBuff array to the MyFreeList pointer. 
    //It casts the address to the type struct malloc_stc* to ensure proper pointer arithmetic.
    MyFreeList = (struct malloc_stc *)MyBuff;
    MyFreeList->next = NULL;
    //It calculates the size of the free storage available in the buffer.
    MyFreeList->size = sizeof(MyBuff) - sizeof(struct malloc_stc);
    //It assigns the address of the first byte of usable storage in the buffer to the buffer pointer of the MyFreeList node.
    //It calculates this address by adding the size of the struct malloc_stc structure to the address of MyBuff.
    //we effectively move the pointer forward by sizeof(struct malloc_stc) bytes.
    MyFreeList->buffer = (unsigned char *)(MyBuff + sizeof(struct malloc_stc));
    MyAllocatedList = NULL;
}

//find a free block in the free list that can accommodate the requested size. If the free block is larger 
//than the requested size, it splits the block into an allocated block and a smaller remaining free block. 
void *MyMalloc(int size)
{
    struct malloc_stc *current = MyFreeList;
    struct malloc_stc *prev = NULL;

    while (current != NULL)
    {
        if (current->size >= size)
        {
            //Check if there is enough space in the current block to split it 
            //into two blocks: one allocated block and one remaining free block.
            if (current->size - size > sizeof(struct malloc_stc))
            {
                struct malloc_stc *newBlock = (struct malloc_stc *)(current->buffer + size);
                newBlock->next = current->next;
                //to represent the remaining free block size.
                newBlock->size = current->size - size - sizeof(struct malloc_stc);
                 //to point to the beginning of the remaining free block.
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
            //return a pointer to the allocated block by casting current->buffer to void*.
            return (void *)current->buffer;
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

//By incorporating coalescing, the MyFree function merges adjacent free blocks, which helps prevent fragmentation and 
//improves the efficiency of future allocations.
void MyFree(void *buffer)
{
    if (buffer != NULL)
    {
        //Initialize two pointers, current and prev, to traverse the MyAllocatedList and locate the allocated block associated with the buffer pointer.
        struct malloc_stc *current = MyAllocatedList;
        struct malloc_stc *prev = NULL;

        // Find the allocated block associated with the buffer pointer.
        while (current != NULL && current->buffer != buffer)
        {
            prev = current;
            current = current->next;
        }

        //If a matching allocated block is found, remove it from the MyAllocatedList. If prev is NULL, it means the allocated block is the first node in the list, 
        //so update MyAllocatedList to point to the next node. Otherwise, update the next pointer of the previous node to skip the current block.
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
                //This is done by comparing the buffer address of the current block plus its size plus the size of the struct malloc_stc 
                //with the buffer address of the next block. If they are adjacent, they can be merged.
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
