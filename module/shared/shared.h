#pragma once

#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/version.h>
#include <asm/pgtable.h>

#include "../pteditor.h"
#include "../types.h"

extern void (*ptedit_shared_invalidate_tlb)(unsigned long);
extern unsigned long (*ptedit_shared_kallsyms_lookup_name)(const char *name);

int ptedit_shared_initialize_symbols(void);
void ptedit_shared_invalidate_tlb_custom(void *addr);
