#include "../arch.h"

int ptedit_arch_establish_success(
  struct kretprobe_instance *rp,
  struct pt_regs *regs
) {
  if (regs->regs[0] == 0) {
    regs->regs[0] = 1;
  }

  return 0;
}
