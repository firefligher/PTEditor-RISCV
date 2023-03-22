#include "../arch.h"

void ptedit_arch_invalidate_tlb_kernel(void *addr) {
  pr_warn("Running experimental ptedit_arch_invalidate_tlb_kernel implementation.\n");
  __asm__ __volatile__ ("sfence.vma" : : : "memory");
}
