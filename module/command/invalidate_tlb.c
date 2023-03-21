#include "command.h"

long ptedit_command_invalidate_tlb(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_shared_invalidate_tlb((void *) ioctl_param);
  return 0;
}
