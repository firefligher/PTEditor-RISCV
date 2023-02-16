#include "../arch.h"

size_t ptedit_arch_get_pat(void) {
  size_t value;
  asm volatile ("mrs %0, mair_el1\n" : "=r"(value));
  return value;
}
