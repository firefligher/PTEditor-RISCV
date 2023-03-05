#include <linux/printk.h>

#include "../../config.h"
#include "../../shared/shared.h"
#include "../arch.h"
#include "symbols.h"

#define SYM_FLUSH_TLB_MM_RANGE  "flush_tlb_mm_range"
#define SYM_NATIVE_WRITE_C4     "native_write_cr4"

void (*ptedit_arch_flush_tlb_mm_range)(
  struct mm_struct*,
  unsigned long,
  unsigned long,
  unsigned int,
  bool
);

void (*ptedit_arch_write_cr4)(unsigned long);

int ptedit_arch_initialize_symbols(void) {
  ptedit_arch_flush_tlb_mm_range =
    (void *) ptedit_shared_kallsyms_lookup_name(SYM_FLUSH_TLB_MM_RANGE);

  if (!ptedit_arch_flush_tlb_mm_range) {
    pr_alert("Could not retrieve " SYM_FLUSH_TLB_MM_RANGE " function.\n");
    return 0;
  }

  ptedit_arch_write_cr4 =
    (void *) ptedit_shared_kallsyms_lookup_name(SYM_NATIVE_WRITE_C4);

  if (!ptedit_arch_write_cr4) {
    pr_alert("Could not retrieve " SYM_NATIVE_WRITE_C4 " function.\n");
    return 0;
  }

  return 1;
}
