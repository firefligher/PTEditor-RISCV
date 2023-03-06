#include <asm/current.h>
#include <linux/mm.h>

/*
 * NOTE:  linux/version.h has to be included in order to make the preprocessor
 *        conditionals work.
 */

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  #include <linux/mmap_lock.h>
#endif

#include <linux/pid.h>
#include <linux/printk.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
  #include <linux/rwsem.h>
#endif

#include <linux/sched.h>

#include "../arch/arch.h"
#include "../config.h"
#include "shared.h"

struct mm_struct *ptedit_shared_get_mm(size_t pid) {
  return NULL;
}

int ptedit_shared_lock_vm(void) {
  return 0;
}

int ptedit_shared_resolve_vm(size_t addr, ptedit_vm_t *entry) {
  return 1;
}

int ptedit_shared_unlock_vm(void) {
  return 0;
}

int ptedit_shared_update_vm(ptedit_entry_t* new_entry) {
  return 1;
}
