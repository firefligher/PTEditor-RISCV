#include <linux/version.h>

#include "../arch/arch.h"
#include "../config.h"
#include "../memory/memory.h"

#include "command.h"
#include "internal.h"

long ptedit_command_vm_update(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_entry_t container;
  ptedit_vm_t update;
  pgd_t new_pgd;
  ptedit_p4d_t new_p4d;
  pud_t new_pud;
  pmd_t new_pmd;
  pte_t new_pte;

  from_user(&container, (void *) ioctl_param, sizeof(ptedit_entry_t));

  update.valid = container.valid;

  if (update.valid & PTEDIT_VALID_MASK_PGD) {
    new_pgd = native_make_pgd(container.pgd);
    update.pgd = &new_pgd;
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  if (update.valid & PTEDIT_VALID_MASK_P4D) {
    new_p4d = native_make_p4d(container.p4d);
    update.p4d = &new_p4d;
  }
#endif

  if (update.valid & PTEDIT_VALID_MASK_PUD) {
    new_pud = native_make_pud(container.pud);
    update.pud = &new_pud;
  }

  if (update.valid & PTEDIT_VALID_MASK_PMD) {
    new_pmd = native_make_pmd(container.pmd);
    update.pmd = &new_pmd;
  }

  if (update.valid & PTEDIT_VALID_MASK_PTE) {
    new_pte = native_make_pte(container.pte);
    update.pte = &new_pte;
  }

  return ptedit_vm_update(
    container.vaddr,
    sanitize_pid(container.pid),
    &update
  ) ? 0 : -1;
}
