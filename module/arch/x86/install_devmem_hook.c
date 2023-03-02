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

int ptedit_arch_install_devmem_hook(void) {
  if (active) {
    pr_warn("Called ptedit_arch_install_devmem_hook multiple times.");
    return 1;
  }

  active = (register_kretprobe(&probe) == 0);
  return active;
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
  if (regs->ax == 0) {
    regs->ax = 1;
  }

  return 0;
}
