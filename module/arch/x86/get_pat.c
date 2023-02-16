#include "../arch.h"

size_t ptedit_arch_get_pat(void) {
  int low, high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(0x277));
  return low | (((size_t)high) << 32);
}
