#pragma once

#include "../shared/shared.h"

#ifdef __aarch64__
  #include "arm/shim.h"
#endif

#ifdef __riscv
  #include "riscv/shim.h"
#endif

extern int real_page_size;

/**
 * Stores the physical address of the first level page table, that corresponds
 * to the process with the specified pid, at dst.
 *
 * @param dst The physical address of the root-level page table.
 * @param pid The identifier of the process whose root-level page table is
 *            resolved.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the operation succeeded, otherwise
 *          PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_arch_get_page_root(size_t *dst, pid_t pid);
size_t ptedit_arch_get_pat(void);
int ptedit_arch_initialize_constants(void);
int ptedit_arch_initialize_symbols(void);
int ptedit_arch_install_devmem_hook(void);
void ptedit_arch_invalidate_tlb(void *addr);
void ptedit_arch_invalidate_tlb_kernel(void *addr);
void ptedit_arch_set_pat(size_t pat);

/**
 * Replaces the first level page table, that corresponds to the process with
 * the specified pid, with the specified root.
 *
 * @param pid   The identifier of the process whose root-level page table is
 *              replaced.
 *
 * @param root  The physical address of the new root-level page table.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the operation succeeded, otherwise
 *          PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_arch_set_page_root(pid_t pid, size_t root);
void ptedit_arch_uninstall_devmem_hook(void);
void ptedit_arch_vm_to_user(ptedit_entry_t *user, ptedit_vm_t *vm);
