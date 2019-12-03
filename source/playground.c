#include "memory.h"

// malloc.
// free.
struct test {
    int foo;
    int bar;
    char ba;
    char dd;
    char be;
    char ef;
};

typedef struct small {
    char a;
    char b;
    char c;
} small;

typedef struct full8 {
    unsigned int a;
    unsigned int b;
} full8;

void testFree(void* ptr) {
    free(ptr);
}

void testMalloc() {
    struct small* s1 = (small*)malloc(sizeof(small));
    struct full8* f1 = (full8*)malloc(sizeof(full8));

    printPtr("Small", s1);
    printPtr("Full8", f1);

    s1->a = 69;
    s1->b = 70;
    s1->c = 71;

    f1->a = 0xBEEFBADD;
    f1->b = 0xBEEFDEAD;
}

void monoSyllabicEjaculationsOfMenWhoAreAngryAndOutOfBreath(char* ptr, int length) {
    for (int i = 0; i < length; i += 2, ptr += 2) {
        *((unsigned short*)ptr) = i;
    }
}

void testMalloc15() {
    char* malloc15 = (char*)malloc(15);
    printPtr("malloc16", malloc15);
    monoSyllabicEjaculationsOfMenWhoAreAngryAndOutOfBreath(malloc15, 15);
}

void testMalloc62() {
    char* malloc62 = (char*)malloc(62);
    printPtr("malloc62", malloc62);
    monoSyllabicEjaculationsOfMenWhoAreAngryAndOutOfBreath(malloc62, 62);
}

void testMalloc260() {
    char* malloc260 = (char*)malloc(260);
    printPtr("malloc260", malloc260);
    monoSyllabicEjaculationsOfMenWhoAreAngryAndOutOfBreath(malloc260, 260);
}

void testMalloc60000() {
    char* malloc60000 = (char*)malloc(60000);
    printPtr("malloc60000", malloc60000);
    monoSyllabicEjaculationsOfMenWhoAreAngryAndOutOfBreath(malloc60000, 60000);
}

void add() {
    const char* hw = "helloworld";
    const char* test2 = "Test2";

    (void)hw;
    (void)test2;

    //println(hw);
    //println(test2);

    //struct test* mems = (struct test*)malloc(sizeof(struct test) * count);

    //for (int i = 0; i < count; ++i) {
    //    struct test t;
    //    t.ba = 0xDE;
    //    t.dd = 0xAD;
    //    t.be = 0xBE;
    //    t.ef = 0xEF;
    //    t.foo = i;
    //    t.bar = i * 100;

    //    mems[i] = t;
    //}

}



