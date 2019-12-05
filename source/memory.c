#include "memory.h"

const int PAGE_SIZE = 1024 * 64;
const int SMALL_BLOCK = 8;
const int LARGE_BLOCK = 16;
const int VAR_BLOCK = 64;
const int FREE = 0x1;
const int USED = 0x0;
const int MALLOC_MEMORY_SIZE = sizeof(MallocMemory);
const int MEMORY_BLOCK_SIZE = sizeof(MemoryBlock) - 1;
const int MEMORY_PAGE_LIST_SIZE = sizeof(MemoryPageList);

struct MallocMemory *pages = NULL;
struct MemoryPageList *pageListHead = NULL;
struct MemoryPageList *pageListTail = NULL;
int pageCount = 0;
int startingIdx = 0;
unsigned int* blocks;

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

void addBlockToList(struct MemoryBlock* head, struct MemoryBlock* newTail) {
    while (head->next != NULL) {
        head = head->next;
    }

    head->next = newTail;
}

void initMemoryBlock(struct MemoryBlock* block, int blockSize, int memorySize) {
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
    printci("Max Flags", maxFlags);
    printci("Block Length", block->length);
    printPtr("BlockStart", block->blockStart);
#endif
}

struct MemoryBlock* createMemoryBlock(int blockSize) {
    struct MemoryBlock* newBlock = (struct MemoryBlock*)((char*)(0x1) + (getMemory() * PAGE_SIZE - 1));
    growMemory();
    initMemoryBlock(newBlock, blockSize, PAGE_SIZE);

    struct MemoryPageList *page = (struct MemoryPageList*)malloc(MEMORY_PAGE_LIST_SIZE);

    pageListTail->next = page;
    pageListTail = pageListTail->next;
    pageListTail->block = newBlock;

    return newBlock;
}

// TODO: Say you are going to clean this up, but actually never do it.
// TODO: Its better that way.  You can always blame yourself for past decisions.
void initMallocMemory() {
    pageCount = getMemory();
    int memoryLeft = pageCount * PAGE_SIZE - getStartOfHeapPtr2();
    pages = (MallocMemory*)getStartOfHeapPtr();

    startingIdx = ((unsigned long)pages & 0xFFFF0000) >> 16;

#if PRINT_DEBUG
    printci("Starting Idx", startingIdx);
#endif

    struct MemoryBlock* firstSmall =
        (struct MemoryBlock*)(((char*)pages) + MALLOC_MEMORY_SIZE);

    initMemoryBlock(firstSmall, SMALL_BLOCK, memoryLeft - MALLOC_MEMORY_SIZE);
    pages->smallBlocks = firstSmall;

    // Crazy? Mallocseption
    pageListTail = pageListHead = (struct MemoryPageList*)malloc(MEMORY_PAGE_LIST_SIZE);
    pageListHead->block = firstSmall;

#if PRINT_DEBUG
    printPtr("firstSmall", firstSmall);
#endif
    struct MemoryBlock *firstLarge =
        pages->largeBlocks = createMemoryBlock(LARGE_BLOCK);

#if PRINT_DEBUG
    printPtr("firstLarge", firstLarge);
#endif

    struct MemoryBlock *firstVar =
        pages->varBlocks = createMemoryBlock(VAR_BLOCK);

#if PRINT_DEBUG
    printPtr("firstnVar", firstVar);
    printPtr("pages", pages);
#endif
}

void mallocNextPage() {
    // Ready everything.
    if (pages == NULL) {
        initMallocMemory();
    }
}

#if PRINT_DEBUG
void printFreeStuff(struct MemoryBlock* head, int idx, unsigned char offset) {

    printPtr("Flags", head->flags);
    printci("blockLength", head->length);
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
        printci("#setNextFreeFlag", currPos);
        printFreeStuff(block, idx, offset);
#endif

    } while (currPos < block->length && ((block->flags[idx] >> offset) & 0xFF) != FREE);

    block->firstFree = currPos;
}

bool hasFree(struct MemoryBlock* block) {
    printci("#hasFree", block->length);
    printci("#hasFree", block->firstFree);
    return block->firstFree < block->length;
}

void* malloc(unsigned long size) {
    (void)size;

    if (pages == NULL) {
        mallocNextPage();
    }

    struct MemoryBlock* head = NULL;
    int blockSize = 0;


    // small allocator
    if (size <= SMALL_BLOCK) {
#if PRINT_DEBUG
        printci("MALLOC SMALL", size);
#endif
        head = pages->smallBlocks;
        blockSize = SMALL_BLOCK;
    }
    else if (size <= LARGE_BLOCK) {
        head = pages->largeBlocks;
        blockSize = LARGE_BLOCK;
#if PRINT_DEBUG
        printci("MALLOC LARGE", size);
#endif
    }
    else {
        head = pages->varBlocks;
        blockSize = VAR_BLOCK;
#if PRINT_DEBUG
        printci("MALLOC VAR", size);
#endif
    }

    // Do something?  Eh?  Trap^^^L#___
    if (head == NULL) {
        return NULL;
    }

    void* ptr = NULL;
    unsigned short currPos;
    do {
        currPos = head->firstFree;

#if PRINT_DEBUG
        println("#malloc");
#endif
        if (!hasFree(head)) {

#if PRINT_DEBUG
            println("#malloc does not have any free available, creating new block.");
#endif
            // Allocate a new page.
            if (head->next == NULL) {
                head->next = createMemoryBlock(blockSize);
            }

            head = head->next;
            continue;
        }

        // Harder case, and not done yet
        if (blockSize == VAR_BLOCK) {

            // TODO: REMOVE RIGHT AWAY.
            // this will go away when we have page crossing allocations.
            if (size >= 64536) {
                // THAT IS A NO
                println("NO SOUP FOR YOU");
                return NULL;
            }

            int preLengthCount = size / VAR_BLOCK;
            if (size % VAR_BLOCK > 0) {
                preLengthCount++;
            }

            int breadCrumbLength = 2;
            int atomCount = (size + breadCrumbLength) / VAR_BLOCK;
            if ((size + breadCrumbLength) % VAR_BLOCK > 0) {
                atomCount++;
            }

            // can the current block support the variable length blocks required.
            if (head->length - head->firstFree < atomCount) {
                head->next = createMemoryBlock(blockSize);
                head = head->next;
            }

            unsigned char* startOfAlloc = head->blockStart +
                head->firstFree * blockSize;

            ptr = startOfAlloc + breadCrumbLength;
            *((unsigned short*)startOfAlloc) = atomCount;

            int idx = head->firstFree;
#if PRINT_DEBUG
            printci("firstFree for Var Block", idx);
#endif
            for (int i = 0; i < atomCount; ++i, ++idx) {
                int flagIdx = idx / 8;
                unsigned char offset = 7 - (idx % 8);

#if PRINT_DEBUG
                printci("ZERO FREE flagIdx", flagIdx);
                printci("ZERO FREE offset", offset);
#endif
                head->flags[flagIdx] =
                    (head->flags[flagIdx] & (0xFF ^ (FREE << offset)));
            }
#if PRINT_DEBUG
                printci("set firstFree", idx);
#endif
            head->firstFree = idx;
        }

        // Way easier
        else {

            // use this atom
            int idx = currPos / 8;
            unsigned char offset = 7 - (currPos % 8);

#if PRINT_DEBUG
            printci("atom index", currPos);
            printFreeStuff(head, idx, offset);
#endif
            head->flags[idx] = (head->flags[idx] & (0xFF ^ (FREE << offset)));
            ptr = head->blockStart + currPos * head->blockSize;
        }

        // TODO: We could also allocate the next block
        setNextFreeFlag(head, currPos);

    } while (ptr == NULL && head != NULL);

#if PRINT_DEBUG
    println("#endmalloc");
#endif
    return ptr;
}

// TODO: Probably could be prettier
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
// TODO: VAR BLOCKS
void free(void* ptr) {
    unsigned long ptrValue = (unsigned long)ptr;
    unsigned long ptrRelativeOffset = ptrValue & 0xFFFF;
    unsigned int idx = (unsigned int)(
            (ptrValue & 0xFFFF0000) >> 16) - startingIdx;

#if PRINT_DEBUG
    printPtr("ptr", ptr);
    printci("PtrValue", ptrValue);
    printci("ptrRelativeOffset", ptrRelativeOffset);
    printci("idx", idx);
#endif

    struct MemoryPageList* page = pageListHead;
    for (; idx >= 1; --idx) {
        page = page->next;
    }

    struct MemoryBlock* block = (struct MemoryBlock*)page->block;
    unsigned int atomIdx = (unsigned int)(ptrRelativeOffset -
        ((unsigned long)block->blockStart & 0xFFFF)) / block->blockSize;

#if PRINT_DEBUG
    printPtr("blockOffset", block->blockStart);
    printci("blockSize", block->blockSize);
    printci("atomIdx", atomIdx);
#endif

    unsigned char* atomStart = (block->blockStart + atomIdx * block->blockSize);
    unsigned char* curr = atomStart;
    for (int i = 0; i < block->blockSize; ++i) {
#if PRINT_DEBUG
        printPtr("zeroing", curr);
#endif
        *curr = 0x0;
        curr++;
    }

    int flagIdx = (int)(atomIdx / 8);
    unsigned char flagOffset = 7 - (atomIdx % 8);

    block->flags[flagIdx] =
        (block->flags[flagIdx] & (0xFF ^ (FREE << flagOffset)));

    if (atomIdx < block->firstFree) {
        block->firstFree = atomIdx;
#if PRINT_DEBUG
        printPtr("updatingFirstFree: firstFree", block->blockStart);
#endif
    }

#if PRINT_DEBUG
    printci("flagSettingIdx", flagIdx);
    printci("flagSettingOffset", flagOffset);
#endif
}

