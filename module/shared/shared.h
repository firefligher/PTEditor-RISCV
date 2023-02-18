#pragma once

#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/version.h>

#include "../pteditor.h"

typedef struct {
    size_t pid;
    pgd_t *pgd;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
    p4d_t *p4d;
#else
    size_t *p4d;
#endif
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    size_t valid;
} vm_t;

extern void (*ptedit_shared_invalidate_tlb)(unsigned long);
extern unsigned long (*ptedit_shared_kallsyms_lookup_name)(const char *name);

void ptedit_shared_destroy_device(void);
struct mm_struct *ptedit_shared_get_mm(size_t pid);
int ptedit_shared_initialize_device(void);
int ptedit_shared_initialize_symbols(void);
int ptedit_shared_lock_vm(void);
int ptedit_shared_resolve_vm(size_t addr, vm_t* entry, int lock);
int ptedit_shared_unlock_vm(void);
int ptedit_shared_update_vm(ptedit_entry_t* new_entry, int lock);
