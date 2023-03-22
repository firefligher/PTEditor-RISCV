#include <linux/limits.h>

#include "../arch/arch.h"
#include "../memory/memory.h"
#include "command.h"
#include "internal.h"

long ptedit_command_write_page(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_page_t container;
  void *page;

  if (SIZE_MAX > ULONG_MAX) {
    pr_err("Unsupported scenario: max(size_t) > max(unsigned long).\n");
    return -1;
  }

  from_user(&container, (void *) ioctl_param, sizeof(ptedit_page_t));

  if (!ptedit_page_get(&page, (unsigned long) container.pfn)) {
    return -1;
  }

  from_user(page, container.buffer, real_page_size);
  return 0;
}
