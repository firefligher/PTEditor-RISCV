#include <linux/compiler.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
#include <linux/kallsyms.h>
#endif

#include <linux/kprobes.h>
#include "shared.h"

#define SYM_KALLSYMS_LOOKUP_NAME  "kallsyms_lookup_name"

unsigned long (*ptedit_shared_kallsyms_lookup_name)(const char *name);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
static struct kprobe kp = {
  .symbol_name = SYM_KALLSYMS_LOOKUP_NAME
};
#endif

int ptedit_shared_initialize_symbols(void) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
  register_kprobe(&kp);
  ptedit_shared_kallsyms_lookup_name = (void *) kp.addr;
  unregister_kprobe(&kp);

  if(!unlikely(kallsyms_lookup_name)) {
    pr_alert("Could not retrieve " SYM_KALLSYMS_LOOKUP_NAME " address.\n");
    return 0;
  }
#else
  ptedit_shared_kallsyms_lookup_name = kallsyms_lookup_name;
#endif

  return 1;
}
