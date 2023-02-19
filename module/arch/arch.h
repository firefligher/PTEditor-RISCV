#pragma once

#include <linux/kprobes.h>

#include "../pteditor.h"
#include "../shared/shared.h"

#ifdef __aarch64__
  #include "arm/shim.h"
#endif

#ifdef __riscv
  #include "riscv/shim.h"
#endif

extern int real_page_size;

int ptedit_arch_establish_success(
  struct kretprobe_instance *rp,
  struct pt_regs *regs
);

size_t ptedit_arch_get_pat(void);
int ptedit_arch_initialize_constants(void);
int ptedit_arch_initialize_symbols(void);
void ptedit_arch_invalidate_tlb(void *addr);
void ptedit_arch_invalidate_tlb_kernel(unsigned long addr);
void ptedit_arch_set_pat(void *_pat);
void ptedit_arch_vm_to_user(ptedit_entry_t *user, vm_t *vm);
