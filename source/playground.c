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

void add(int count) {
    const char* hw = "helloworld";
    const char* test2 = "Test2";

    println(hw);
    println(test2);

    struct test* mems = (struct test*)malloc(sizeof(struct test) * count);
    printTest(-2, mems);

    struct test t2;
    printTest(-1, &t2);

    for (int i = 0; i < count; ++i) {
        struct test t;
        t.ba = 0xDE;
        t.dd = 0xAD;
        t.be = 0xBE;
        t.ef = 0xEF;
        t.foo = i;
        t.bar = i * 100;

        mems[i] = t;
    }

    printTests(count, mems);
}



