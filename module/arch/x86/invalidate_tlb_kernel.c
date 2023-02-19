#include <asm/current.h>
#include <linux/sched.h>

#include "../../shared/shared.h"
#include "../arch.h"
#include "symbols.h"

void ptedit_arch_invalidate_tlb_kernel(unsigned long addr) {
  ptedit_arch_flush_tlb_mm_range(
    ptedit_shared_get_mm(task_pid_nr(current)),
    addr,
    addr + real_page_size,
    12,
    false
  );
}
