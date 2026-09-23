#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  int sz = 32 * 4096;
  char *mem = sbrk(sz);

  if ((long)mem == -1) {
    printf("sbrk failed\n");
    exit(1);
  }

  char prefix[] = "Here it is: ";
  int prefix_len = strlen(prefix);

  // Scan our newly allocated memory 
  for (int i = 0; i < sz - prefix_len; i++) {
    int match = 1;
    
    // Check if the prefix exists 
    for (int j = 0; j < prefix_len; j++) {
      if (mem[i + j] != prefix[j]) {
        match = 0;
        break;
      }
    }

    if (match) {
      char next_char = mem[i + prefix_len];
      
      // Check if the character immediately following the prefix is alphanumeric.
      // guarantees we are looking at the dynamically generated string on the 
      // heap.
      if ((next_char >= 'A' && next_char <= 'Z') || 
          (next_char >= 'a' && next_char <= 'z') || 
          (next_char >= '0' && next_char <= '9')) {
        
        // We found the actual heap payload! Print the secret.
        printf("%s\n", &mem[i + prefix_len]);
        exit(0);
      }
    }
  }

  // If we didn't find it, exit with an error. 
  exit(1);
}
