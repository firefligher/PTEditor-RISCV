#include "../arch.h"

void ptedit_arch_invalidate_tlb_kernel(void *addr) {
  /*
   * At least on the Tina Kernel, there is no accessible flush function from
   * the Kernel, that we could call here. Instead, we rely on our own.
   */

  ptedit_arch_invalidate_tlb(addr);
}
