#include <asm/current.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  #include <linux/mmap_lock.h>
#endif

#include <linux/pgtable.h>
#include <linux/pid.h>
#include <linux/printk.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
  #include <linux/rwsem.h>
#endif

#include <linux/sched.h>

#include "../arch/arch.h"
#include "../pteditor.h"
#include "shared.h"

static bool mm_is_locked = false;

struct mm_struct *ptedit_shared_get_mm(size_t pid) {
  struct task_struct *task;
  struct pid* vpid;

  /* Find mm */
  task = current;
  if(pid != 0) {
    vpid = find_vpid(pid);
    if(!vpid) return NULL;
    task = pid_task(vpid, PIDTYPE_PID);
    if(!task) return NULL;
  }
  if(task->mm) {
      return task->mm;
  } else {
      return task->active_mm;
  }
  return NULL;
}

int ptedit_shared_lock_vm(void) {
  struct mm_struct *mm = current->active_mm;
  
  if(mm_is_locked) {
    pr_warn("VM is already locked\n");
    return 0;
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  mmap_write_lock(mm);
  mmap_read_lock(mm);
#else
  down_write(&mm->mmap_sem);
  down_read(&mm->mmap_sem);
#endif

  mm_is_locked = true;
  return 1;
}

int ptedit_shared_resolve_vm(size_t addr, vm_t *entry) {
  struct mm_struct *mm;
  bool lock = !mm_is_locked;

  if(!entry) return 1;
  entry->pud = NULL;
  entry->pmd = NULL;
  entry->pgd = NULL;
  entry->pte = NULL;
  entry->p4d = NULL;
  entry->valid = 0;

  mm = ptedit_shared_get_mm(entry->pid);
  if(!mm) {
      return 1;
  }

  /* Lock mm */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  if(lock) mmap_read_lock(mm);
#else
  if(lock) down_read(&mm->mmap_sem);
#endif

  /* Return PGD (page global directory) entry */
  entry->pgd = pgd_offset(mm, addr);
  if (pgd_none(*(entry->pgd)) || pgd_bad(*(entry->pgd))) {
      entry->pgd = NULL;
      goto error_out;
  }
  entry->valid |= PTEDIT_VALID_MASK_PGD;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  /* Return p4d offset */
  entry->p4d = p4d_offset(entry->pgd, addr);
  if (p4d_none(*(entry->p4d)) || p4d_bad(*(entry->p4d))) {
    entry->p4d = NULL;
    goto error_out;
  }
  entry->valid |= PTEDIT_VALID_MASK_P4D;

  /* Get offset of PUD (page upper directory) */
  entry->pud = pud_offset(entry->p4d, addr);
  if (pud_none(*(entry->pud))) {
    entry->pud = NULL;
    goto error_out;
  }
  entry->valid |= PTEDIT_VALID_MASK_PUD;
#else
  /* Get offset of PUD (page upper directory) */
  entry->pud = pud_offset(entry->pgd, addr);
  if (pud_none(*(entry->pud))) {
    entry->pud = NULL;
    goto error_out;
  }
  entry->valid |= PTEDIT_VALID_MASK_PUD;
#endif


  /* Get offset of PMD (page middle directory) */
  entry->pmd = pmd_offset(entry->pud, addr);
  if (pmd_none(*(entry->pmd)) || pud_large(*(entry->pud))) {
    entry->pmd = NULL;
    goto error_out;
  }
  entry->valid |= PTEDIT_VALID_MASK_PMD;

  /* Map PTE (page table entry) */
  entry->pte = pte_offset_map(entry->pmd, addr);
  if (entry->pte == NULL || pmd_large(*(entry->pmd))) {
    entry->pte = NULL;
    goto error_out;
  }
  entry->valid |= PTEDIT_VALID_MASK_PTE;

  /* Unmap PTE, fine on x86 and ARM64 -> unmap is NOP */
  pte_unmap(entry->pte);

  /* Unlock mm */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  if(lock) mmap_read_unlock(mm);
#else
  if(lock) up_read(&mm->mmap_sem);
#endif

  return 0;

error_out:

  /* Unlock mm */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  if(lock) mmap_read_unlock(mm);
#else
  if(lock) up_read(&mm->mmap_sem);
#endif

  return 1;
}

int ptedit_shared_unlock_vm(void) {
  struct mm_struct *mm = current->active_mm;
  
  if(!mm_is_locked) {
    pr_warn("VM is not locked\n");
    return 0;
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  mmap_write_unlock(mm);
  mmap_read_unlock(mm);
#else
  up_write(&mm->mmap_sem);
  up_read(&mm->mmap_sem);
#endif

  mm_is_locked = false;
  return 1;
}

int ptedit_shared_update_vm(ptedit_entry_t* new_entry) {
  vm_t old_entry;
  size_t addr = new_entry->vaddr;
  struct mm_struct *mm = ptedit_shared_get_mm(new_entry->pid);
  int lock = !mm_is_locked;
  if(!mm) return 1;

  old_entry.pid = new_entry->pid;

  /* Lock mm */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  if(lock) mmap_write_lock(mm);
#else
  if(lock) down_write(&mm->mmap_sem);
#endif

  ptedit_shared_resolve_vm(addr, &old_entry, 0);

  /* Update entries */
  if((old_entry.valid & PTEDIT_VALID_MASK_PGD) && (new_entry->valid & PTEDIT_VALID_MASK_PGD)) {
      pr_warn("Updating PGD\n");
      set_pgd(old_entry.pgd, native_make_pgd(new_entry->pgd));
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  if((old_entry.valid & PTEDIT_VALID_MASK_P4D) && (new_entry->valid & PTEDIT_VALID_MASK_P4D)) {
      pr_warn("Updating P4D\n");
      set_p4d(old_entry.p4d, native_make_p4d(new_entry->p4d));
  }
#endif

  if((old_entry.valid & PTEDIT_VALID_MASK_PUD) && (new_entry->valid & PTEDIT_VALID_MASK_PUD)) {
      pr_warn("Updating PUD\n");
      set_pud(old_entry.pud, native_make_pud(new_entry->pud));
  }

  if((old_entry.valid & PTEDIT_VALID_MASK_PMD) && (new_entry->valid & PTEDIT_VALID_MASK_PMD)) {
      pr_warn("Updating PMD\n");
      set_pmd(old_entry.pmd, native_make_pmd(new_entry->pmd));
  }

  if((old_entry.valid & PTEDIT_VALID_MASK_PTE) && (new_entry->valid & PTEDIT_VALID_MASK_PTE)) {
      pr_warn("Updating PTE\n");
      set_pte(old_entry.pte, native_make_pte(new_entry->pte));
  }

  ptedit_shared_invalidate_tlb(addr);

  /* Unlock mm */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  if(lock) mmap_write_unlock(mm);
#else
  if(lock) up_write(&mm->mmap_sem);
#endif

  return 0;
}
