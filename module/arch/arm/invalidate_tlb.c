#include "../arch.h"

void ptedit_arch_invalidate_tlb(void *addr) {
  asm volatile ("dsb ishst");
  asm volatile ("tlbi vmalle1is");
  asm volatile ("dsb ish");
  asm volatile ("isb");
}
