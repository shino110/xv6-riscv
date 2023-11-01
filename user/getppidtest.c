#include "kernel/types.h"
#include "user/user.h"
int main() {
    int c = fork();
    if (c == 0) {
        printf("CHILD: fork()=%d, getpid()=%d, getppid()=%d\n", c, getpid(), getppid());
    }
    else {
        wait(0);
        printf("PARENT: fork()=%d, getpid()=%d, getppid()=%d\n", c, getpid(), getppid());
    }
    exit(0);
}
