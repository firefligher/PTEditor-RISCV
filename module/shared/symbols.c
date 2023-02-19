#include <linux/compiler.h>

/*
 * Including linux/version.h here is required, because we need to determine
 * what other includes we need. This depends on the Kernel version.
 */

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 7, 0)
#include <linux/kallsyms.h>
#endif

#include <linux/kprobes.h>
#include <linux/printk.h>

#include "../arch/arch.h"
#include "shared.h"

#define SYM_KALLSYMS_LOOKUP_NAME  "kallsyms_lookup_name"

unsigned long (*ptedit_shared_kallsyms_lookup_name)(const char *name);
void (*ptedit_shared_invalidate_tlb)(unsigned long);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
static struct kprobe kp = {
  .symbol_name = SYM_KALLSYMS_LOOKUP_NAME
};
#endif

int ptedit_shared_initialize_symbols(void) {
  ptedit_shared_invalidate_tlb = ptedit_arch_invalidate_tlb_kernel;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
  register_kprobe(&kp);
  ptedit_shared_kallsyms_lookup_name = (void *) kp.addr;
  unregister_kprobe(&kp);

  if(!unlikely(ptedit_shared_kallsyms_lookup_name)) {
    pr_alert("Could not retrieve " SYM_KALLSYMS_LOOKUP_NAME " address.\n");
    return 0;
  }
#else
  ptedit_shared_kallsyms_lookup_name = kallsyms_lookup_name;
#endif

  return 1;
}
