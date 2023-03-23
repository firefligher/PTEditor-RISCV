#include <linux/kprobes.h>
#include <linux/printk.h>

#include "../arch.h"

#define SYMBOL_DEVMEM_IS_ALLOWED  "devmem_is_allowed"

static int establish_success(
  struct kretprobe_instance *rp,
  struct pt_regs *regs
);

static struct kretprobe probe = {
  .handler = establish_success,
  .maxactive = 20,
  .kp = { .symbol_name = SYMBOL_DEVMEM_IS_ALLOWED }
};

static int active = 0;

ptedit_status_t ptedit_arch_install_devmem_hook(void) {
  if (active) {
    pr_warn("Called ptedit_arch_install_devmem_hook multiple times.");
    return PTEDIT_STATUS_SUCCESS;
  }

  active = (register_kretprobe(&probe) == 0);

  return (active)
    ? PTEDIT_STATUS_SUCCESS
    : PTEDIT_STATUS_ERROR;
}

void ptedit_arch_uninstall_devmem_hook(void) {
  if (!active) {
    return;
  }

  unregister_kretprobe(&probe);
  active = 0;
}

static int establish_success(
  struct kretprobe_instance *rp,
  struct pt_regs *regs
) {
  if (regs->regs[0] == 0) {
    regs->regs[0] = 1;
  }

  return 0;
}
