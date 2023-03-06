#include <linux/mm.h>
#include <linux/uaccess.h>

#include "../arch/arch.h"
#include "../config.h"
#include "internal.h"
#include "memory.h"

ptedit_status_t ptedit_vm_resolve(ptedit_vm_t *dst, void *addr, pid_t pid) {
  struct mm_struct *mm;

  if (!(mm = internal_acquire_mm(pid))) {
    return PTEDIT_STATUS_ERROR;
  }

  memset(dst, 0, sizeof(ptedit_vm_t));

  /* Read PGD (page global directory) entry */
  dst->pgd = pgd_offset(mm, (uintptr_t) addr);

  if (pgd_none(*(dst->pgd)) || pgd_bad(*(dst->pgd))) {
      dst->pgd = NULL;
      goto error_out;
  }

  dst->valid |= PTEDIT_VALID_MASK_PGD;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
  /* Read offset of PUD (page upper directory) */
  dst->pud = pud_offset(dst->pgd, (uintptr_t) addr);

  if (pud_none(*(dst->pud))) {
    dst->pud = NULL;
    goto error_out;
  }

  dst->valid |= PTEDIT_VALID_MASK_PUD;
#else
  /* Read p4d offset */
  dst->p4d = p4d_offset(dst->pgd, (uintptr_t) addr);

  if (p4d_none(*(dst->p4d)) || p4d_bad(*(dst->p4d))) {
    dst->p4d = NULL;
    goto error_out;
  }

  dst->valid |= PTEDIT_VALID_MASK_P4D;

  /* Read offset of PUD (page upper directory) */
  dst->pud = pud_offset(dst->p4d, (uintptr_t) addr);

  if (pud_none(*(dst->pud))) {
    dst->pud = NULL;
    goto error_out;
  }

  dst->valid |= PTEDIT_VALID_MASK_PUD;
#endif

  /* Read offset of PMD (page middle directory) */
  dst->pmd = pmd_offset(dst->pud, (uintptr_t) addr);

  if (pmd_none(*(dst->pmd)) || pud_large(*(dst->pud))) {
    dst->pmd = NULL;
    goto error_out;
  }

  dst->valid |= PTEDIT_VALID_MASK_PMD;

  /* Read PTE map (page table entry) */
  dst->pte = pte_offset_map(dst->pmd, (uintptr_t) addr);

  if (dst->pte == NULL || pmd_large(*(dst->pmd))) {
    dst->pte = NULL;
    goto error_out;
  }

  dst->valid |= PTEDIT_VALID_MASK_PTE;

  /* Unmap PTE, fine on x86, ARM64 and RISC V -> unmap is NOP */
  pte_unmap(dst->pte);

  internal_release_mm(pid);
  return PTEDIT_STATUS_SUCCESS;

error_out:
  internal_release_mm(pid);
  return PTEDIT_STATUS_ERROR;
}
