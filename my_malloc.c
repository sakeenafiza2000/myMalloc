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
//unsigned char represents a byte of data
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

//find a free block in the free list that can accommodate the requested size. If the free block is larger 
//than the requested size, it splits the block into an allocated block and a smaller remaining free block. 
void *MyMalloc(int size)
{
    struct malloc_stc *current = MyFreeList;

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
                newBlock->prev = current;
                //to represent the remaining free block size.
                newBlock->size = current->size - size - sizeof(struct malloc_stc);
                //to point to the beginning of the remaining free block.
                newBlock->buffer = current->buffer + size + sizeof(struct malloc_stc);
                if (current->next != NULL)
                    current->next->prev = newBlock;
                current->next = newBlock;
            }
            //Update the pointers and size of the allocated block (current):
            //current->size is set to the requested size to indicate the allocated block size.
            current->size = size;
            //MyFreeList is updated to current->next since the current block has been allocated and should be removed from the free list.
            MyFreeList = current->next;
            //return a pointer to the allocated block by casting current->buffer to void*.
            return (void *)current->buffer;
        }
        current = current->next;
    }

    return NULL;
}

//add the freed block back to the free list, allowing it to be reused for future allocations.
void MyFree(void *buffer)
{
    if (buffer != NULL)
    {
        //Calculate the address of the block structure (struct malloc_stc) associated with the buffer pointer. 
        struct malloc_stc *block = (struct malloc_stc *)((unsigned char *)buffer - sizeof(struct malloc_stc));
        // links the freed block to the beginning of the free list.
        block->next = MyFreeList;
        block->prev = NULL;
        if (MyFreeList != NULL)
            MyFreeList->prev = block;
        MyFreeList = block;
    }
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