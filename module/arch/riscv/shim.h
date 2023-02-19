#pragma once

#include <asm/pgtable.h>

static inline pte_t native_make_pte(pteval_t val) {
  return __pte(val);
}

static inline pgd_t native_make_pgd(pgdval_t val) {
  return __pgd(val);
}

static inline pmd_t native_make_pmd(pmdval_t val) {
  return __pmd(val);
}

static inline pud_t native_make_pud(pudval_t val) {
  return __pud(val);
}

static inline p4d_t native_make_p4d(p4dval_t val) {
  return __p4d(val);
}

static inline pteval_t native_pte_val(pte_t pte) {
  return pte_val(pte);
}
