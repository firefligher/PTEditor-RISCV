#include "../arch/arch.h"
#include "command.h"

long ptedit_command_get_page_size(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
    return real_page_size;
}
