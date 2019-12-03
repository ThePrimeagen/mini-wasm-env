#pragma once

#include "wasm.h"

#if !defined(NULL)
#define NULL 0
#endif

#if !defined(false)
#define false 0
#define true 1
#endif

#if !defined(PRINT_DEBUG)
#define PRINT_DEBUG true
#endif

#if !__has_builtin(__builtin_wasm_memory_grow)
#define __builtin_wasm_memory_grow(v, k) false
#endif

#if !__has_builtin(__builtin_wasm_memory_size)
#define __builtin_wasm_memory_size(v) false
#endif

extern void* getStartOfHeapPtr();
extern int getStartOfHeapPtr2();
extern int resetMemoryGlobals();

// TODO: Quickly write a hack ass malloc
// 64KB
typedef struct MemoryBlock {
    unsigned short length;
    unsigned short firstFree;
    unsigned short blockSize;

    // the linked list of blocks of the same size
    struct MemoryBlock *next;

    // WHAT ELSE
    // TYPESCRIPT AUTO GENERATION
    unsigned char* blockStart;
    unsigned char flags[1]; // Assuming everything is 1 byte for length.
} MemoryBlock;

typedef struct MallocMemory {
    struct MemoryBlock *smallBlocks;
    struct MemoryBlock *largeBlocks;
    struct MemoryBlock *varBlocks;
} MallocMemory;

typedef struct MemoryPageList {
    struct MemoryPageList* next;
    void* block;
} MemoryPageList;

void* malloc(unsigned long size);
void free(void *ptr);

