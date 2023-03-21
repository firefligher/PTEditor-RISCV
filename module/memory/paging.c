#include <linux/mm.h>
#include <linux/printk.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include "../arch/arch.h"
#include "../config.h"
#include "../shared/shared.h"
#include "internal.h"
#include "memory.h"

ptedit_status_t ptedit_page_get(void **dst, unsigned long pfn) {
  if (!pfn_valid(pfn)) {
    pr_warn("Attempted to resolve invalid page frame number: '%lu'\n", pfn);
    return PTEDIT_STATUS_ERROR;
  }

  *dst = phys_to_virt(pfn * real_page_size);
  return PTEDIT_STATUS_SUCCESS;
}

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

ptedit_status_t ptedit_vm_update(void *addr, pid_t pid, ptedit_vm_t *value) {
  ptedit_vm_t *entry;

  if (!ptedit_vm_lock(pid)) {
    return PTEDIT_STATUS_ERROR;
  }

  if (!ptedit_vm_resolve(&entry, addr, pid)) {
    ptedit_vm_unlock(pid);
    return PTEDIT_STATUS_ERROR;
  }

  if (vm.valid & value.valid & PTEDIT_VALID_MASK_PGD) {
    pr_warn("Updating PGD\n");
    set_pgd(entry.pgd, value->pgd);
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  if (vm.valid & value.valid & PTEDIT_VALID_MASK_P4D) {
    pr_warn("Updating P4D\n");
    set_p4d(entry.p4d, value->p4d);
  }
#endif

  if (vm.valid & value.valid & PTEDIT_VALID_MASK_PUD) {
    pr_warn("Updating PUD\n");
    set_pud(entry.pud, value->pud);
  }

  if (vm.valid & value.valid & PTEDIT_VALID_MASK_PMD) {
    pr_warn("Updating PMD\n");
    set_pmd(entry.pmd, value->pmd);
  }

  if (vm.valid & value.valid & PTEDIT_VALID_MASK_PTE) {
    pr_warn("Updating PTE\n");
    set_pte(entry.pte, value->pte);
  }

  ptedit_shared_invalidate_tlb(addr);
  ptedit_vm_unlock(pid);
}
