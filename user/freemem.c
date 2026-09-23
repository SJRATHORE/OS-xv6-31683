#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  printf("Free memory: %ld bytes\n", freemem());
  exit(0);
}
