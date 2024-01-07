#include <stdio.h>
#include <stdint.h>

int main(void)
{
  // example of using 8bytes of memory as a holder
  // for two 4byte ints
  uint64_t i = 0xffffffffffffff;
  i = i & 0xffffffff00000000;
  i += (int)2;

  i = i & 0x00000000ffffffff;
  i += 7l << 32;

  int lower_4bytes = (int)i;
  int upper_4bytes = (int)(i >> 32);

  printf("i as int from lower 4bytes -> %d\n", lower_4bytes);
  printf("i as int from upper 4bytes -> %d\n", upper_4bytes);
  printf("i as uint64 -> %llu\n", (uint64_t)i);
}
