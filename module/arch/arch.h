#pragma once

#include "../pteditor.h"
#include "../shared/shared.h"

extern const int real_page_size;
extern const int real_page_shift;

int ptedit_arch_initialize_constants(void);
int ptedit_arch_initialize_symbols(void);
void ptedit_arch_invalidate_tlb(void *addr);
void ptedit_arch_invalidate_tlb_kernel(unsigned long addr);
void ptedit_arch_set_pat(void *_pat);
void ptedit_arch_vm_to_user(ptedit_entry_t *user, vm_t *vm);
