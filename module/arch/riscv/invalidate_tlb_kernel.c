#include "../arch.h"

void ptedit_arch_invalidate_tlb_kernel(void *addr) {
  pr_warn("Running experimental ptedit_arch_invalidate_tlb_kernel implementation.\n");
  asm volatile("sfence.vma" ::: "memory");
}
