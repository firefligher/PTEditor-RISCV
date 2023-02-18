#include <asm/io.h>

/*
 * NOTE:  linux/sched.h has to be included before asm/uaccess.h due to the
 *        latter depnding on the definition of 'struct task_struct'.
 */

#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
  #include <linux/mmap_lock.h>
#endif

#include <linux/printk.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
  #include <linux/rwsem.h>
#endif

#include <linux/types.h>

#include "../arch/arch.h"
#include "../pteditor.h"
#include "shared.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  #define from_user raw_copy_from_user
  #define to_user raw_copy_to_user
#else
  #define from_user copy_from_user
  #define to_user copy_to_user
#endif

static long device_ioctl(
  struct file *file,
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static void invalidate_tlb_custom(unsigned long addr);
static void set_pat(size_t pat);

static bool device_busy = false;

static struct file_operations f_ops = {.owner = THIS_MODULE,
                                       .unlocked_ioctl = device_ioctl,
                                       .open = device_open,
                                       .release = device_release};

static struct miscdevice misc_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = PTEDITOR_DEVICE_NAME,
    .fops = &f_ops,
    .mode = S_IRWXUGO,
};

/* Global stuff */

void ptedit_shared_destroy_device(void) {
  misc_deregister(&misc_dev);
}

int ptedit_shared_initialize_device(void) {
  int r;

  r = misc_register(&misc_dev);

  if (r != 0) {
    pr_alert("Failed registering device with %d\n", r);
    return 0;
  }

  return 1;
}

/* Static stuff */

static long device_ioctl(
  struct file *file,
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  switch (ioctl_num) {
    case PTEDITOR_IOCTL_CMD_VM_RESOLVE:
    {
        ptedit_entry_t vm_user;
        vm_t vm;
        (void)from_user(&vm_user, (void*)ioctl_param, sizeof(vm_user));
        vm.pid = vm_user.pid;
        ptedit_shared_resolve_vm(vm_user.vaddr, &vm, !mm_is_locked);
        ptedit_arch_vm_to_user(&vm_user, &vm);
        (void)to_user((void*)ioctl_param, &vm_user, sizeof(vm_user));
        return 0;
    }
    case PTEDITOR_IOCTL_CMD_VM_UPDATE:
    {
        ptedit_entry_t vm_user;
        (void)from_user(&vm_user, (void*)ioctl_param, sizeof(vm_user));
        ptedit_shared_update_vm(&vm_user, !mm_is_locked);
        return 0;
    }
    case PTEDITOR_IOCTL_CMD_VM_LOCK:
    {
      return ptedit_shared_lock_vm() ? 0 : -1;
    }
    case PTEDITOR_IOCTL_CMD_VM_UNLOCK:
    {
      return ptedit_shared_unlock_vm() ? 0 : -1;
    }
    case PTEDITOR_IOCTL_CMD_READ_PAGE:
    {
        ptedit_page_t page;
        (void)from_user(&page, (void*)ioctl_param, sizeof(page));
        to_user(page.buffer, phys_to_virt(page.pfn * real_page_size), real_page_size);
        return 0;
    }
    case PTEDITOR_IOCTL_CMD_WRITE_PAGE:
    {
        ptedit_page_t page;
        (void)from_user(&page, (void*)ioctl_param, sizeof(page));
        (void)from_user(phys_to_virt(page.pfn * real_page_size), page.buffer, real_page_size);
        return 0;
    }
    case PTEDITOR_IOCTL_CMD_GET_ROOT:
    {
      /*
        struct mm_struct *mm;
        ptedit_paging_t paging;

        (void)from_user(&paging, (void*)ioctl_param, sizeof(paging));
        mm = ptedit_shared_get_mm(paging.pid);

#if defined(__aarch64__)
        if(!mm || (mm && !mm->pgd)) {
            // M1 Asahi Linux workaround with the limitation that it only works for the current process
            asm volatile("mrs %0, ttbr0_el1" : "=r" (paging.root));
            paging.root &= ~1;
            (void)to_user((void*)ioctl_param, &paging, sizeof(paging));
            return 0;
        }
#endif

        if(!mm) return 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
        if(!mm_is_locked) mmap_read_lock(mm);
#else
        if(!mm_is_locked) down_read(&mm->mmap_sem);
#endif
        paging.root = virt_to_phys(mm->pgd);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
        if(!mm_is_locked) mmap_read_unlock(mm);
#else
        if(!mm_is_locked) up_read(&mm->mmap_sem);
#endif
        (void)to_user((void*)ioctl_param, &paging, sizeof(paging));
        return 0;
        */
        return -1;
    }
    case PTEDITOR_IOCTL_CMD_SET_ROOT:
    {
      /*
        struct mm_struct *mm;
        ptedit_paging_t paging = {0};

        (void)from_user(&paging, (void*)ioctl_param, sizeof(paging));
        mm = ptedit_shared_get_mm(paging.pid);
        if(!mm) return 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
        if(!mm_is_locked) mmap_write_lock(mm);
#else
        if(!mm_is_locked) down_write(&mm->mmap_sem);
#endif
        mm->pgd = (pgd_t*)phys_to_virt(paging.root);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
        if(!mm_is_locked) mmap_write_unlock(mm);
#else
        if(!mm_is_locked) up_write(&mm->mmap_sem);
#endif
        return 0;
      */
      return -1;
    }
    case PTEDITOR_IOCTL_CMD_GET_PAGESIZE:
        return real_page_size;
    case PTEDITOR_IOCTL_CMD_INVALIDATE_TLB:
        ptedit_shared_invalidate_tlb(ioctl_param);
        return 0;
    case PTEDITOR_IOCTL_CMD_GET_PAT:
    {
      size_t value = ptedit_arch_get_pat();
      (void)to_user((void*)ioctl_param, &value, sizeof(value));
      return 0;
    }
    case PTEDITOR_IOCTL_CMD_SET_PAT:
    {
        set_pat(ioctl_param);
        return 0;
    }
    case PTEDITOR_IOCTL_CMD_SWITCH_TLB_INVALIDATION:
    {
      if((int)ioctl_param != PTEDITOR_TLB_INVALIDATION_KERNEL && (int)ioctl_param != PTEDITOR_TLB_INVALIDATION_CUSTOM)
        return -1;
      ptedit_shared_invalidate_tlb = ((int)ioctl_param == PTEDITOR_TLB_INVALIDATION_KERNEL) ? ptedit_arch_invalidate_tlb_kernel : invalidate_tlb_custom;
      return 0;
    }

    default:
        return -1;
  }

  return 0;
}

static int device_open(struct inode *inode, struct file *file) {
  /* Check if device is busy */
  if (device_busy == true) {
    return -EBUSY;
  }

  device_busy = true;
  return 0;
}

static int device_release(struct inode *inode, struct file *file) {
  /* Unlock module */
  device_busy = false;
  return 0;
}

static void invalidate_tlb_custom(unsigned long addr) {
  on_each_cpu(ptedit_arch_invalidate_tlb, (void*) addr, 1);
}

static void set_pat(size_t pat) {
  on_each_cpu(ptedit_arch_set_pat, (void*) pat, 1);
}
