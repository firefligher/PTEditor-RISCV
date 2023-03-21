#include "../arch/arch.h"
#include "command.h"

long ptedit_command_set_pat(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_arch_set_pat((size_t) ioctl_param);
  return 0;
}
