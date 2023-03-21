#include <linux/version.h>

#include "../arch/arch.h"
#include "../memory/memory.h"
#include "command.h"

long ptedit_command_vm_update(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_entry_t *container = (ptedit_entry_t *) ioctl_param;
  ptedit_vm_t update;

  update.valid = container.valid;

  if (update.valid & PTEDIT_VALID_MASK_PGD) {
    update.pgd = native_make_pgd(container->pgd);
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  if (update.valid & PTEDIT_VALID_MASK_P4D) {
    update.p4d = native_make_p4d(container->p4d);
  }
#endif

  if (update.valid & PTEDIT_VALID_MASK_PUD) {
    update.pud = native_make_pud(container->pud);
  }

  if (update.valid & PTEDIT_VALID_MASK_PMD) {
    update.pmd = native_make_pmd(container->pmd);
  }

  if (update.valid & PTEDIT_VALID_MASK_PTE) {
    update.pte = native_make_pte(container->pte);
  }

  return (ptedit_vm_update(container->vaddr, container->pid, update))
    ? 0 : -1;
}
