#include "kernel/types.h"
#include "user/user.h"

int main() {
  printf("%l\n", freemem());
  sbrk(1);
  printf("%l\n", freemem());
  sbrk(4095);
  printf("%l\n", freemem());
  sbrk(1);
  printf("%l\n", freemem());
  sbrk(-4096);
  printf("%l\n", freemem());
  sbrk(-1);
  printf("%l\n", freemem());
  exit(0);
}
