#pragma once

#include "wasm.h"

#if !defined(NULL)
#define NULL 0
#endif

#if !defined(false)
#define false 0
#define true 1
#endif

extern void* getStartOfHeapPtr();
extern void* getPageOfMemory();

// TODO: QUickly write a hack ass malloc
typedef struct MemoryBlock {
    struct MemoryBlock* next;
    struct MemoryBlock* prev;

    int size;
    char free;

    // WHAT ELSE
    // TYPESCRIPT AUTO GENERATION
    char mem[1]; // Assuming everything is 1 byte for length.
} MemoryBlock;

void* malloc(unsigned long size);
void free(void *ptr);
void mallocNextPage();

