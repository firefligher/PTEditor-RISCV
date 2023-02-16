#include "../arch.h"

void ptedit_arch_set_pat(void *_pat) {
  size_t pat = (size_t)_pat;
  asm volatile ("msr mair_el1, %0\n" : : "r"(pat));
}
