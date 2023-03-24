#include <linux/mm.h>
#include <linux/version.h>
#include "../arch.h"

void ptedit_arch_vm_to_user(ptedit_entry_t *user, ptedit_vm_t *vm) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#if CONFIG_PGTABLE_LEVELS > 4
  if(vm->p4d) user->p4d = (vm->p4d)->p4d;
#else
#if !defined(__ARCH_HAS_5LEVEL_HACK)
  if(vm->p4d) user->p4d = (vm->p4d)->pgd.pgd;
#else
  if(vm->p4d) user->p4d = (vm->p4d)->pgd;
#endif
#endif
#endif

  if(vm->pgd) user->pgd = pgd_val(*(vm->pgd));
  if(vm->pmd) user->pmd = pmd_val(*(vm->pmd));
  if(vm->pud) user->pud = pud_val(*(vm->pud));
  if(vm->pte) user->pte = pte_val(*(vm->pte));

  user->valid = vm->valid;
}
