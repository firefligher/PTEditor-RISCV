#include "../../memory/memory.h"
#include "../arch.h"

/**
 * NOTE:  Shamelessly copied from x86. Requires validation.
 */

ptedit_status_t ptedit_arch_get_page_root(size_t *dst, pid_t pid) {
  struct mm_struct *mm;

  if (!ptedit_mm_acquire(&mm, pid)) {
    return PTEDIT_STATUS_ERROR;
  }

  *dst = (size_t) virt_to_phys(mm->pgd);
  ptedit_mm_release(pid);

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
