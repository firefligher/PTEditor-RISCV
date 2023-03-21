#include "../arch/arch.h"
#include "command.h"
#include "internal.h"

long ptedit_command_get_root(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_paging_t container;
  from_user(&container, (void *) ioctl_param, sizeof(ptedit_paging_t));

  if (!ptedit_arch_get_page_root(&container.root, container.pid)) {
    return -1;
  }

  to_user((void *) ioctl_param, &container, sizeof(ptedit_paging_t));
  return 0;
}
