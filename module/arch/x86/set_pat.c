#include "../arch.h"

void ptedit_arch_set_pat(size_t pat) {
  int low, high;
  low = pat & 0xffffffff;
  high = (pat >> 32) & 0xffffffff;
  asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(0x277));
}
