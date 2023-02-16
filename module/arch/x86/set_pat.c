#include "../arch.h"

void ptedit_arch_set_pat(void *_pat) {
  int low, high;
  size_t pat = (size_t)_pat;
  low = pat & 0xffffffff;
  high = (pat >> 32) & 0xffffffff;
  asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(0x277));
}
