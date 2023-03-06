#pragma once

#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/version.h>
#include <asm/pgtable.h>

#include "../pteditor.h"
#include "../types.h"

extern void (*ptedit_shared_invalidate_tlb)(unsigned long);
extern unsigned long (*ptedit_shared_kallsyms_lookup_name)(const char *name);
/*
 * TBD: Add explanation.
 */

struct mm_struct *ptedit_shared_get_mm(size_t pid);
int ptedit_shared_initialize_symbols(void);
int ptedit_shared_lock_vm(void);
int ptedit_shared_unlock_vm(void);
int ptedit_shared_update_vm(ptedit_entry_t* new_entry);
