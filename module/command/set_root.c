#include "../arch/arch.h"
#include "command.h"
#include "internal.h"

long ptedit_command_set_root(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_paging_t container;
  from_user(&container, (void *) ioctl_param, sizeof(ptedit_paging_t));

  return (ptedit_arch_set_page_root(container.pid, container.root))
    ? 0
    : -1;
}
