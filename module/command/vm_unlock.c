#include "command.h"
#include "internal.h"
#include "../memory/memory.h"

long ptedit_command_vm_unlock(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  return ptedit_vm_unlock(
    sanitize_pid(0),
    PTEDIT_VM_UNLOCK_FAIL_ON_STILL_LOCKED
  ) ? 0 : -1;
}
