// pingpong.c for xv6 (assignment #1 for CSC.T371)
// name: Shino Ito
// id: 21B30066

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// <<<remove this comment and fill your code here if needed>>>

int main(int argc, char *argv[]) {
    if (argc != 2) {
    fprintf(1, "usage:␣%s␣N\n", argv[0]);
    exit(1);
    }

    // # of rounds
    int n = atoi(argv[1]);

    // tick value before starting rounds
    int start_tick = uptime();
    
    int p[2];
    pipe(p);

    //Peterson Algorithm
    volatile bool wantp = false, wantq = false;
    volatile int last = 0;
    int pid = fork();
    if (pid == 0){
        for (int i = 0; i < n; i++) {
            wantq = true;
            last = 1;
            while (wantp && last == 1);
            char *in;
            read(p[0], in, 1);
            *in += 1;
            write(p[1], in, 1);
            wantq = false;
        }
        exit(0);
    }else {
        char *in;
        *in = "0";
        write(p[1], in, 1);
        wantp = false;
        for (int i = 1; i < n; i++) {
            wantp = true;
            last = 0;
            while(wantq && last == 0);
            char *in;
            read(p[0], in, 1);
            *in += 1;
            write(p[1], in, 1);
            wantp = false;
        }

        wait(pid);
    }


    // tick value after ending rounds
    int end_tick = uptime();
    // print # of ticks in nrounds
    printf("#␣of␣ticks␣in␣%d␣rounds:␣%d\n", n, end_tick - start_tick);
    exit(0);
}