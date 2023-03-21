#include "../arch.h"

size_t ptedit_arch_get_pat(void) {
  int low, high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(0x277));
  return low | (((size_t)high) << 32);
}

void ptedit_arch_set_pat(size_t pat) {
  int low, high;
  low = pat & 0xffffffff;
  high = (pat >> 32) & 0xffffffff;
  asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(0x277));
}
