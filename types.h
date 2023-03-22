#pragma once

#include "config.h"

#if PTEDIT_MODULE_BUILD
  #include <linux/uaccess.h>
  #include <linux/version.h>
#else
  #include <stddef.h>
#endif

/*
 * The Tina kernel needs the following include for different page table level
 * types.
 */

#if PTEDIT_TINA_BUILD
  #include <linux/mm.h>
#endif

/*
 * Some wrappers for types that may be absent.
 */

#if PTEDIT_ON_LINUX
  typedef pid_t ptedit_pid_t;
#endif

#if PTEDIT_ON_WINDOWS
  typedef size_t ptedit_pid_t;
#endif

#if PTEDIT_MODULE_BUILD
  typedef enum {
    PTEDIT_STATUS_ERROR = 0,
    PTEDIT_STATUS_SUCCESS
  } ptedit_status_t;
#endif

#define PTEDIT_VALID_MASK_PGD (1 << 0)
#define PTEDIT_VALID_MASK_P4D (1 << 1)
#define PTEDIT_VALID_MASK_PUD (1 << 2)
#define PTEDIT_VALID_MASK_PMD (1 << 3)
#define PTEDIT_VALID_MASK_PTE (1 << 4)

#if PTEDIT_MODULE_BUILD && PTEDIT_HAS_P4D
  typedef p4d_t ptedit_p4d_t;
#elif PTEDIT_MODULE_BUILD
  typedef size_t ptedit_p4d_t;
#endif

#if PTEDIT_MODULE_BUILD
  typedef struct {
    pgd_t *pgd;
    ptedit_p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    size_t valid;
  } ptedit_vm_t;
#endif

/**
 * Structure containing the page-table entries of all levels.
 * The Linux names are aliased with the Intel names.
 */
typedef struct {
  /** Process ID */
  ptedit_pid_t pid;

  /** Virtual address */
  void *vaddr;

  /** Page global directory / Page map level 5 */
  union {
    size_t pgd;
    size_t pml5;
  };

  /** Page directory 4 / Page map level 4 */
  union {
    size_t p4d;
    size_t pml4;
  };

  /** Page upper directory / Page directory pointer table */
  union {
    size_t pud;
    size_t pdpt;
  };

  /** Page middle directory / Page directory */
  union {
    size_t pmd;
    size_t pd;
  };

  /** Page table entry */
  size_t pte;

  /** Bitmask indicating which entries are valid/should be updated */
  size_t valid;
} ptedit_entry_t;

#if PTEDIT_ON_LINUX
  /**
   * Structure to read/write physical pages.
   */
  typedef struct {
    /** Page-frame number */
    size_t pfn;

    /** Virtual address */
    size_t vaddr;

    /** Page size */
    size_t size;

    /** Page content */
    unsigned char* buffer;
  } ptedit_page_t;
#endif

typedef enum {
  PTEDIT_TLB_INVALIDATION_CUSTOM = 1,
  PTEDIT_TLB_INVALIDATION_KERNEL = 0
} ptedit_tlb_invalidation_t;

#define PTEDITOR_TLB_INVALIDATION_CUSTOM  (PTEDIT_TLB_INVALIDATION_CUSTOM)
#define PTEDITOR_TLB_INVALIDATION_KERNEL  (PTEDIT_TLB_INVALIDATION_KERNEL)

/**
 * Structure to get/set the root of paging
 */
typedef struct {
  /** Process id */
  ptedit_pid_t pid;

  /** Physical address of paging root */
  size_t root;
} ptedit_paging_t;
