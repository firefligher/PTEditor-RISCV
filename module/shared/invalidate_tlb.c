#include <linux/smp.h>
#include "../arch/arch.h"
#include "shared.h"

void ptedit_shared_invalidate_tlb_custom(void *addr) {
  on_each_cpu(ptedit_arch_invalidate_tlb, addr, 1);
}
