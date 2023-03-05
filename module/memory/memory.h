#pragma once

#include "../types.h"

/**
 * Resolves the corresponding page table entries for the specified virtual
 * address in the context of the process with specified pid.
 *
 * @param dst   The location where the resolved information is stored.
 * @param addr  The virtual address whose page table entries are resolved.
 * @param pid   The identifier of the process that is the owner of the virtual
 *              address space.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the resolution was successful and
 *          the result has been stored at dst, otherwise PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_vm_resolve(ptedit_vm_t *dst, void *addr, pid_t pid);
