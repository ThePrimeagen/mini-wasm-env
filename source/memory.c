#include "memory.h"

const int PAGE_SIZE = 1024 * 64;

const int SMALL_BLOCK = 8;
const int LARGE_BLOCK = 16;
const int VAR_BLOCK = 64;
const int FREE = 0x1;
const int USED = 0x0;
const int MEMORY_PAGES_SIZE = sizeof(MemoryPages);
const int MEMORY_BLOCK_SIZE = sizeof(MemoryBlock) - 1;

struct MemoryPages *pages = NULL;
int pageCount = 0;

int getMemory() {
    return __builtin_wasm_memory_size(0);
}

void growMemory() {
    __builtin_wasm_memory_grow(0, 1);
    pageCount = getMemory();
    resetMemoryGlobals();

#if PRINT_DEBUG
    printi(pageCount);
#endif
}

void initMemoryBlock(struct MemoryBlock* block, int blockSize, int memorySize) {
    (void)block;
    (void)blockSize;
    (void)memorySize;

#if PRINT_DEBUG
    printPtr("initMemoryBlock", block);
#endif

    int memoryBlocksMax = PAGE_SIZE / blockSize;
    int maxFlags = memoryBlocksMax / 8;

    // Memset?
    unsigned char* flags = block->flags;
    for (int i = 0; i < maxFlags; ++i, ++flags) {
        *flags = 0xFF;
    }

    block->length = (memorySize - MEMORY_BLOCK_SIZE - maxFlags) / blockSize;
    block->blockStart = flags;
    block->blockSize = blockSize;

#if PRINT_DEBUG
    printi(maxFlags);
    printi(block->length);
    printPtr("BlockStart", block->blockStart);
#endif
}

void initMemoryPages() {
    pageCount = getMemory();
    int memoryLeft = pageCount * PAGE_SIZE - getStartOfHeapPtr2();
    pages = (MemoryPages*)getStartOfHeapPtr();

    struct MemoryBlock* firstSmall =
        (struct MemoryBlock*)(((char*)pages) + MEMORY_PAGES_SIZE);

    initMemoryBlock(firstSmall, SMALL_BLOCK, memoryLeft - MEMORY_PAGES_SIZE);
    pages->smallBlocks = firstSmall;

#if PRINT_DEBUG
    printPtr("firstSmall", firstSmall);
#endif

    growMemory();
    // off by one
    struct MemoryBlock* firstLarge =
        (struct MemoryBlock*)((char*)pages + memoryLeft);
    initMemoryBlock(firstLarge, LARGE_BLOCK, PAGE_SIZE);
    pages->largeBlocks = firstLarge;

#if PRINT_DEBUG
    printPtr("firstLarge", firstLarge);
#endif

    growMemory();
    struct MemoryBlock* firstVar =
        (struct MemoryBlock*)((char*)pages + memoryLeft + PAGE_SIZE);
    initMemoryBlock(firstVar, VAR_BLOCK, PAGE_SIZE);
    pages->varBlocks = firstVar;

#if PRINT_DEBUG
    printPtr("firstnVar", firstVar);
    printPtr("pages", pages);
#endif
}

void mallocNextPage() {
    // Ready everything.
    if (pages == NULL) {
        initMemoryPages();
    }
}

#if PRINT_DEBUG
void printFreeStuff(struct MemoryBlock* head, int idx, unsigned char offset) {

    printPtr("Flags", head->flags);
    printci("Checking flag", idx);
    printci("Offset", offset);
    printci("flagValue", head->flags[idx]);
    printci("Value", head->flags[idx] & (FREE << offset));
    printci("Value Shifted", (head->flags[idx] >> offset) & 0xFF);
    printci("Value Equal Free", ((head->flags[idx] >> offset) & 0xFF) == FREE);
    println("-------------------");
}
#endif

void setNextFreeFlag(struct MemoryBlock* block, int currPos) {

    int idx;
    unsigned char offset;

    do {
        currPos++;
        idx = currPos / 8;
        offset = 7 - (currPos % 8);

#if PRINT_DEBUG
        println("#setNextFreeFlag");
        printFreeStuff(block, idx, offset);
#endif

    } while(currPos < block->length && ((block->flags[idx] >> offset) & 0xFF) != FREE);

    block->firstFree = currPos;
}

void* malloc(unsigned long size) {
    (void)size;

    if (pages == NULL) {
        mallocNextPage();
    }

    struct MemoryBlock* head = NULL;

    // small allocator
    if (size <= SMALL_BLOCK) {
        head = pages->smallBlocks;
    }

    if (head == NULL) {
        return NULL;
    }

    void* ptr = NULL;
    unsigned short currPos = head->firstFree;
    do {
        if (currPos >= head->length) {
            head = head->next;
            continue;
        }

        int idx = currPos / 8;
        unsigned char offset = 7 - (currPos % 8);

#if PRINT_DEBUG
        println("#malloc");
        printFreeStuff(head, idx, offset);
#endif
        // use this atom
        head->flags[idx] = (head->flags[idx] & (0xFF ^ (FREE << offset)));
        ptr = head->blockStart + currPos * head->blockSize;

        // TODO: We could also allocate the next block
        setNextFreeFlag(head, currPos);

    } while (ptr == NULL && head != NULL);

    //if (size <
    return ptr;
}

void free(void* ptr) {
    // todo: OOPS
    (void)ptr;
}




