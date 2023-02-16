#include <linux/cpufeature.h>
#include <linux/invpcid.h>
#include <linux/irqflags.h>
#include <linux/paravirt.h>
#include <linux/percpu.h>
#include <linux/processor-flags.h>
#include <linux/tlbflush.h>
#include <linux/version.h>

#include "../arch.h"
#include "symbols.h"

void ptedit_arch_invalidate_tlb(void *addr) {
  int pcid;
  unsigned long flags;
  unsigned long cr4;

#if defined(X86_FEATURE_INVPCID_SINGLE) && defined(INVPCID_TYPE_INDIV_ADDR) // <1>
  if (cpu_feature_enabled(X86_FEATURE_INVPCID_SINGLE)) {
    for(pcid = 0; pcid < 4096; pcid++) {
      invpcid_flush_one(pcid, (long unsigned int) addr);
    }
  } 
  else 
#endif                                                                      // </1>
  {
    raw_local_irq_save(flags);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)                            // <2>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)                            // <3>
    cr4 = native_read_cr4();
#else                                                                       // :3:
    cr4 = this_cpu_read(cpu_tlbstate.cr4);
#endif                                                                      // </3>
#else                                                                       // :2:
    cr4 = __read_cr4();
#endif                                                                      // </2>
    ptedit_arch_write_cr4(cr4 & ~X86_CR4_PGE);
    ptedit_arch_write_cr4(cr4);
    raw_local_irq_restore(flags);
  }
}
