#include <asm/current.h>
#include <asm/mmu_context.h>
#include <linux/printk.h>
#include <linux/sched.h>

#include "../../memory/memory.h"
#include "../arch.h"

/**
 * Vaguely adopted from
 * https://github.com/Tina-Linux/tina-d1x-linux-5.4/blob/master/arch/riscv/mm/tlbflush.c
 */

void ptedit_arch_invalidate_tlb(void *addr) {
  pid_t pid;
  struct mm_struct *mm;
  int asid;

  if (!ptedit_mm_acquire(&mm, pid = task_pid_nr(current))) {
    pr_warn("Cannot invalidate TLB with kernel strategy.\n");
    return;
  }

  asid = cpu_asid(mm);

  /*
   * NOTE:  Although the Tina Kernel aligns the virtual address (addr) to a
   *        page boundary first, acccording to the RISC-V manual, this is not
   *        required. Hence, we omit it here.
   */

  asm volatile("sfence.vma %0, %1" :: "r"(addr), "r"(asid) : "memory");
  ptedit_mm_release(pid);
}
