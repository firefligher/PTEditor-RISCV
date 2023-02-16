#pragma once

#include <asm/page.h>
#include <linux/hugetlb.h>

static inline pte_t native_make_pte(pteval_t val)
{
  return __pte(val);
}

static inline pgd_t native_make_pgd(pgdval_t val)
{
  return __pgd(val);
}

static inline pmd_t native_make_pmd(pmdval_t val)
{
  return __pmd(val);
}

static inline pud_t native_make_pud(pudval_t val)
{
  return __pud(val);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
static inline p4d_t native_make_p4d(p4dval_t val)
{
  return __p4d(val);
}
#endif

static inline pteval_t native_pte_val(pte_t pte)
{
  return pte_val(pte);
}

static inline int pud_large(pud_t pud) {
#ifdef __PAGETABLE_PMD_FOLDED 
    return pud_val(pud) && !(pud_val(pud) & PUD_TABLE_BIT);
#else
    return 0;
#endif
}

static inline int pmd_large(pmd_t pmd) {
#ifdef __PAGETABLE_PMD_FOLDED
    return pmd_val(pmd) && !(pmd_val(pmd) & PMD_TABLE_BIT)
#else
    return 0;
#endif
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
typedef pgdval_t p4dval_t;
#endif

void __attribute__((weak)) set_swapper_pgd(pgd_t* pgdp, pgd_t pgd) {}
pgd_t __attribute__((weak)) swapper_pg_dir[PTRS_PER_PGD];
