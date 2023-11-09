// pingpong.c for xv6 (assignment #1 for CSC.T371)
// name: Shino Ito
// id: 21B30066

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// <<<remove this comment and fill your code here if needed>>>

int main(int argc, char *argv[]) {
    // # of rounds
    int n = atoi(argv[1]);

    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        for (int i = 0; i < n; i++) {
            unsigned char in = 0;
            read(p[0], &in, 1);
            in += 1;
            write(p[1], &in, 1);
        }
        exit(0);
    } else {
        unsigned char in = 0;
        write(p[1], &in, 1);
        for (int i = 1; i < n; i++) {
            unsigned char in = 0;
            read(p[0], &in, 1);
            in += 1;
            write(p[1], &in, 1);
        }
        wait(0);
    }
    
    exit(0);
}