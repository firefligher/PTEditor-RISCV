#include "../arch.h"

int ptedit_arch_establish_success(
  struct kretprobe_instance *rp,
  struct pt_regs *regs
) {
  if (regs->ax == 0) {
    regs->ax = 1;
  }

  return 0;
}
