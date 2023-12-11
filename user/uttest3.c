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
void foo2() {
    for (;;) {
        printf("foo2␣(tid=%d):␣%d\n", mytid(), c);
        c += 1;
        uthread_wait(&c);
    }
}
void bar() {
    printf("bar␣(tid=%d):␣%d\n", mytid(), c);
    c += 2;
    uthread_notify(foo_tid, &c);
    uthread_exit();
}
void bar2() {
    printf("bar2␣(tid=%d):␣%d\n", mytid(), c);
    c += 2;
    uthread_notify_all(&c);
    uthread_exit();
}

int main() {
    make_uthread(foo);
    make_uthread(bar);
    start_uthreads();
    c = 0;
    make_uthread(foo2);
    make_uthread(bar2);
    start_uthreads();
    exit(0);
}