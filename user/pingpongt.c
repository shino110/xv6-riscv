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

    int ticks = exec_time("/pingponga", argv); //returned # of ticks 

    // print # of ticks in nrounds
    printf("#␣ticks␣in␣%d␣rounds:␣%d\n", n, ticks);
    exit(0);
}