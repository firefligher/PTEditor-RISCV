#include "../arch.h"

size_t ptedit_arch_get_pat(void) {
  size_t value;
  asm volatile ("mrs %0, mair_el1\n" : "=r"(value));
  return value;
}

void ptedit_arch_set_pat(size_t pat) {
  asm volatile ("msr mair_el1, %0\n" : : "r"(pat));
}
