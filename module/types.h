#pragma once

#ifdef __linux__
  #include <linux/types.h>
  #include <linux/uaccess.h>
  #include <linux/version.h>
#else
  #include <stddef.h>
#endif

#if defined(PTEDIT_MODULE_BUILD) && defined(PTEDIT_TINA_FIXES)
  #include <linux/mm.h>
#endif

#ifdef __linux__
  typedef pid_t ptedit_pid_t;
#else
  typedef size_t ptedit_pid_t;
#endif

#ifdef PTEDIT_MODULE_BUILD
  typedef enum {
    PTEDIT_STATUS_ERROR = 0,
    PTEDIT_STATUS_SUCCESS
  } ptedit_status_t;
#endif

#if defined(PTEDIT_MODULE_BUILD) && \
    defined(__linux__) && \
    LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
  typedef size_t ptedit_p4d_t;
#elif defined(PTEDIT_MODULE_BUILD) && defined(__linux__)
  typedef p4d_t ptedit_p4d_t;
#endif

#if defined(PTEDIT_MODULE_BUILD) && defined(__linux__)
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
