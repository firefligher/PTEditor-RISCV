#include <linux/mm_types.h>
#include <asm/tlbflush.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/fs.h>
#include <linux/kallsyms.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/ptrace.h>
#include <linux/proc_fs.h>
#include <linux/kprobes.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
#include <linux/mmap_lock.h>
#endif

#ifdef CONFIG_PAGE_TABLE_ISOLATION
pgd_t __attribute__((weak)) __pti_set_user_pgtbl(pgd_t *pgdp, pgd_t pgd);
#endif

#include "config.h"
#include "pteditor.h"
#include "arch/arch.h"
#include "shared/shared.h"

MODULE_AUTHOR("Michael Schwarz");
MODULE_DESCRIPTION("Device to play around with paging structures");
MODULE_LICENSE("GPL");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)

/**
 * We need the following namespace import in order to be able to use
 * kernel_read.
 */

MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static struct proc_ops umem_ops = {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
  .proc_flags = 0,
#endif
  .proc_open = NULL,
  .proc_read = NULL,
  .proc_write = NULL,
  .proc_lseek = NULL,
  .proc_release = NULL,
  .proc_poll = NULL,
  .proc_ioctl = NULL,
#ifdef CONFIG_COMPAT
  .proc_compat_ioctl = NULL,
#endif
  .proc_mmap = NULL,
  .proc_get_unmapped_area = NULL,
};
#define OP_lseek lseek
#define OPCAT(a, b) a ## b
#define OPS(o) OPCAT(umem_ops.proc_, o)
#else
static struct file_operations umem_ops = {.owner = THIS_MODULE};
#define OP_lseek llseek
#define OPS(o) umem_ops.o
#endif

static int open_umem(struct inode *inode, struct file *filp) { return 0; }
static int has_umem = 0;

static int __init pteditor_init(void) {
  // ORDER MATTERS!

  if (!ptedit_shared_initialize_symbols() ||
      !ptedit_arch_initialize_symbols() ||
      !ptedit_shared_initialize_device() ||
      !ptedit_arch_initialize_constants()) {
    return -ENXIO;
  }

  if (!ptedit_arch_install_devmem_hook()) {
    pr_alert("Could not bypass /dev/mem restriction\n");
  } else {
    pr_info("/dev/mem is now superuser read-/writable\n");
  }

  /* Premature return for the sake of testing. */

  return 0;

  OPS(OP_lseek) = (void*)ptedit_shared_kallsyms_lookup_name("memory_lseek");
  OPS(read) = (void*)ptedit_shared_kallsyms_lookup_name("read_mem");
  OPS(write) = (void*)ptedit_shared_kallsyms_lookup_name("write_mem");
  OPS(mmap) = (void*)ptedit_shared_kallsyms_lookup_name("mmap_mem");
  OPS(open) = open_umem;

  if (!OPS(OP_lseek) || !OPS(read) || !OPS(write) ||
      !OPS(mmap) || !OPS(open)) {
    pr_alert("Could not create unprivileged memory access\n");
  } else {
    proc_create("umem", 0666, NULL, &umem_ops);
    pr_info("Unprivileged memory access via /proc/umem set up\n");
    has_umem = 1;
  }
  pr_info("Loaded.\n");

  return 0;
}

static void __exit pteditor_exit(void) {
  ptedit_shared_destroy_device();
  ptedit_arch_uninstall_devmem_hook();

  if (has_umem) {
    pr_info("Remove unprivileged memory access\n");
    remove_proc_entry("umem", NULL);
  }
  pr_info("Removed.\n");
}

module_init(pteditor_init);
module_exit(pteditor_exit);
