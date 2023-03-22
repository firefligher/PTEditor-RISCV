#include <linux/printk.h>

#include "../arch/arch.h"
#include "../shared/shared.h"
#include "command.h"

long ptedit_command_switch_tlb_invalidation(
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  ptedit_tlb_invalidation_t new_strategy =
    (ptedit_tlb_invalidation_t) ioctl_param;

  switch (new_strategy) {
  case PTEDIT_TLB_INVALIDATION_CUSTOM:
    ptedit_shared_invalidate_tlb = ptedit_shared_invalidate_tlb_custom;
    break;

  case PTEDIT_TLB_INVALIDATION_KERNEL:
    ptedit_shared_invalidate_tlb = ptedit_arch_invalidate_tlb_kernel;
    break;

  default:
    pr_warn(
      "Attempted to switch to unknown TLB invalidation strategy: '%lu'\n",
      ioctl_param
    );

    return -1;
  }

  return 0;
}
