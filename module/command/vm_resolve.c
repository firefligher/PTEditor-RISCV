#include "../arch/arch.h"
#include "../memory/memory.h"
#include "../types.h"
#include "command.h"
#include "internal.h"

long ptedit_command_vm_resolve(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_entry_t entry;
  ptedit_vm_t vm;

  from_user(&entry, (void *) ioctl_param, sizeof(ptedit_entry_t));

  if (
    !ptedit_vm_resolve(&vm, entry.vaddr, entry.pid ? entry.pid : current->pid)
  ) {
    return -1;
  }

  ptedit_arch_vm_to_user(&entry, &vm);
  to_user((void *) ioctl_param, &entry, sizeof(ptedit_entry_t));
  return 0;
}
