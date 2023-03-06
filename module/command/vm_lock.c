#include "command.h"
#include "internal.h"
#include "../memory/memory.h"

long ptedit_command_vm_lock(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  return (ptedit_vm_lock(sanitize_pid(0)))
    ? 0
    : -1;
}
