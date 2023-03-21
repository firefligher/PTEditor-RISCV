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
