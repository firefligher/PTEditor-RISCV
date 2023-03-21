#pragma once

#include "../types.h"

/**
 * Locks the mm_struct instance of the specified pid and stores the
 * corresponding pointer at dst.
 *
 * Once acquired, the mm_struct instance must be released again in order to
 * avoid deadlocks. The acquire-strategy is reentrant-safe, if locking and
 * unlocking are performed symmetrically.
 *
 * @param dst The destination for the acquired mm_struct instance.
 * @param pid The PID of the process whose corresponding mm_struct instance
 *            will be acquired.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the operation succeeded, otherwise
 *          PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_mm_acquire(struct mm_struct **dst, pid_t pid);

/**
 * Unlocks the mm_struct instance of the process that corresponds to the
 * specified pid.
 *
 * This operation fails silently: If nothing can be unlocked due to lacking a
 * corresponding lock, this is ignored.
 *
 * @param pid The PID of the process whose mm_struct is unlocked.
 */
void ptedit_mm_release(pid_t pid);

/**
 * Retrieves a pointer to the page with the specified page frame number and
 * stores it in the specified dst.
 *
 * For compatibility reasons, this function is only capable of resolving
 * pointers to pages that match the real_page_size. There is no support for
 * pages of other sizes and hence callers end up accessing only pieces of a
 * certain page, i.e. in case of a huge page.
 *
 * @param dst The destination for the resolved pointer.
 * @param pfn The page frame number of the page.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the operation succeeded, otherwise
            PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_page_get(void **dst, unsigned long pfn);

/**
 * Locks the memory management for the process corresponding to the specified
 * pid.
 *
 * This lock is reentrant-safe. All locking operations must have a
 * corresponding ptedit_vm_unlock, otherwise the usual lock issues may occur.
 *
 * @param pid The pid of the process whose memory will be locked.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if locking was successful, otherwise
 *          PTEDIT_STATUS_ERROR, if the memory management for the pid cannot be
 *          resolved.
 */
ptedit_status_t ptedit_vm_lock(pid_t pid);

/**
 * Resolves the corresponding page table entries for the specified virtual
 * address in the context of the process with the specified pid.
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

#define PTEDIT_VM_UNLOCK_FAIL_ON_STILL_LOCKED ((unsigned int) 0x01)

/**
 * Unlocks the memory management for the process corresponding to the specified
 * pid.
 *
 * If PTEDIT_VM_UNLOCK_FAIL_ON_STILL_LOCKED is set in flags, the corresponding
 * function call only succeeds, if the memory management for the specific
 * process can be obtained by other parties again.
 *
 * @param pid   The pid of the process whose memory will be unlocked.
 * @param flags Bitmask of flags that control the behavior of this function.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if unlocking was successful,
 *          otherwise PTEDIT_STATUS_ERROR, if the memory management for the pid
 *          cannot be resolved.
 */
ptedit_status_t ptedit_vm_unlock(pid_t pid, unsigned int flags);

/**
 * Updates the page table entries that correspond to the specified virtual
 * address within the context of the process with the specified pid.
 *
 * @param addr  The virtual address whose page table entries are updated.
 * @param pid   The identifier of the process that is the owner of the virtual
 *              address space.
 *
 * @param value The new information that is written.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if performing the request page table
 *          update succeeded, otherwise PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_vm_update(void *addr, pid_t pid, ptedit_vm_t *value);
