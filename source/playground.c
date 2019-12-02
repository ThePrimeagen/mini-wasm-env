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



