#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthread.h"

int c;
int foo_tid;

void foo() {
    foo_tid = mytid();
    for (;;) {
        printf("foo␣(tid=%d):␣%d\n", mytid(), c);
        c += 1;
        uthread_wait(&c);
    }
}
void bar() {
    for (int i = 0; i < 3; i++) {
        printf("bar␣(tid=%d):␣%d\n", mytid(), c);
        yield();
        c += 2;
    }
    uthread_notify(foo_tid, &c);
    uthread_exit();
}
void baz_sub(int *cp) {
    printf("baz␣(tid=%d):␣%d\n", mytid(), *cp);
    yield();
    *cp += 3;
}
void baz() {
    for (int i = 0; i < 4; i++) {
        baz_sub(&c);
        baz_sub(&c);
    }
    uthread_exit();
}

int main() {
    make_uthread(foo);
    make_uthread(bar);
    make_uthread(baz);
    start_uthreads();
    exit(0);
}