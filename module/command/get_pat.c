#include "../arch/arch.h"
#include "command.h"
#include "internal.h"

long ptedit_command_get_pat(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  size_t pat = ptedit_arch_get_pat();
  to_user((void *) ioctl_param, &pat, sizeof(size_t));
  return 0;
}
