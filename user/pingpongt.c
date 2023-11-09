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

    // CLIENT TIME before starting rounds
    // int start_mtime = mtime();

    int ticks = exec_time("/pingponga", argv); //returned # of ticks 

    // CLIENT TIME after ending rounds
    // int end_mtime = mtime();

    // from start.c 
    // int interval = 1000000; // cycles; about 1/10th second in qemu. 
    // -> 1 second = 10000000 ?
    // int integer = (end_mtime - start_mtime) / 10000;
    // int decimal = (end_mtime - start_mtime) % 10000;

    // print # of ticks in nrounds
    // printf("#␣seconds␣in␣%d␣rounds:␣%d.%04d\n", n, integer, decimal);
    printf("#␣ticks␣in␣%d␣rounds:␣%d\n", n, ticks);
    exit(0);
}