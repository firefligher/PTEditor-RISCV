#pragma once

#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/version.h>

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

struct mm_struct* ptedit_shared_get_mm(size_t pid);
