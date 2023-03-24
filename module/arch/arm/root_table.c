#include "../../memory/memory.h"
#include "../arch.h"

ptedit_status_t ptedit_arch_get_page_root(size_t *dst, pid_t pid) {
  struct mm_struct *mm;

  if (ptedit_mm_acquire(&mm, pid)) {
    if (mm->pgd) {
      *dst = (size_t) virt_to_phys(mm->pgd);
      ptedit_mm_release(pid);
      return PTEDIT_STATUS_SUCCESS;
    }

    ptedit_mm_release(pid);
  }

  /*
   * M1 Asahi Linux workaround with the limitation that it only works for the
   * current process.
   */

  asm volatile("mrs %0, ttbr0_el1" : "=r" (*dst));
  *dst &= ~1;
  return PTEDIT_STATUS_SUCCESS;
}

ptedit_status_t ptedit_arch_set_page_root(pid_t pid, size_t root) {
  struct mm_struct *mm;

  if (!ptedit_mm_acquire(&mm, pid)) {
    return PTEDIT_STATUS_ERROR;
  }

  mm->pgd = (pgd_t *) phys_to_virt(root);
  ptedit_mm_release(pid);

  return PTEDIT_STATUS_SUCCESS;
}
