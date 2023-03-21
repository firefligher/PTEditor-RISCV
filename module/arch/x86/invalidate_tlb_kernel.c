#include <asm/current.h>
#include <linux/printk.h>
#include <linux/sched.h>

#include "../../config.h"
#include "../../memory/memory.h"
#include "../arch.h"
#include "symbols.h"

void ptedit_arch_invalidate_tlb_kernel(void *addr) {
  pid_t pid;
  struct mm_struct *mm;

  if (!ptedit_mm_acquire(&mm, pid = task_pid_nr(current))) {
    pr_warn("Cannot invalidate TLB with kernel strategy.\n");
    return;
  }

  ptedit_arch_flush_tlb_mm_range(
    mm,
    (unsigned long) addr,
    ((unsigned long) addr) + real_page_size,
    12,
    false
  );

  ptedit_mm_release(pid);
}
