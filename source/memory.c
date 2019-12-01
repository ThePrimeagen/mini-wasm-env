#include "memory.h"

const int PAGE_SIZE = 1024 * 64;
const int MEMORY_BLOCK_SIZE = sizeof(MemoryBlock);
const int DEFAULT_MEMORY_SIZE = 256; // Overhead is craaaaazy
const int COMPUTED_MEMORY_SIZE = DEFAULT_MEMORY_SIZE - MEMORY_BLOCK_SIZE; // Overhead is craaaaazy
const int BLOCKS_PER_PAGE = PAGE_SIZE / DEFAULT_MEMORY_SIZE; // Overhead is craaaaazy

MemoryBlock* head = NULL;
MemoryBlock* firstFreeBlock = NULL;
int pageCount = 0;

void grow_memory() {
    pageCount++;
    __builtin_wasm_memory_grow(0, pageCount);
    printi(pageCount);
}

void mallocNextPage() {
    MemoryBlock* ptr;

    // Start with 1 page already allocated.
    if (pageCount == 0) {
        ptr = firstFreeBlock = head = (MemoryBlock*)getStartOfHeapPtr();
        pageCount = 1;
    }

    // allocate the page myself.
    else {
        grow_memory();
        ptr = firstFreeBlock = (MemoryBlock*)
            (((char*)head) + ((pageCount - 1) * PAGE_SIZE));
    }

    MemoryBlock* prev = NULL;
    for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
        MemoryBlock* curr = (MemoryBlock*)ptr;
        curr->free = 1;
        curr->prev = prev;
        curr->size = COMPUTED_MEMORY_SIZE;

        if (prev != NULL) {
            prev->next = curr;
        }

        if (firstFreeBlock == NULL || firstFreeBlock->free == false) {
            firstFreeBlock = curr;
        }
    }
}

void* malloc(unsigned long size) {
    (void)size;
    // TODO: Allocations bigger than a MemoryBlock.
    //     - Merge
    // TODO: A block bigger than continuous memory available blocks.
    //     - Merge entire page allocations?
    if (pageCount == 0) {
        mallocNextPage();
    }

    return head;
}

void free(void* ptr) {
    // todo: OOPS
    (void)ptr;
}

