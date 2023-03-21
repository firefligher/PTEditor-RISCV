#include "../arch.h"

void ptedit_arch_set_pat(size_t pat) {
  asm volatile ("msr mair_el1, %0\n" : : "r"(pat));
}
