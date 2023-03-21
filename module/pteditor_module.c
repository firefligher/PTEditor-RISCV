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

#include "command/command.h"
#include "config.h"
#include "arch/arch.h"
#include "shared/shared.h"
#include "userland/userland.h"

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

static int __init pteditor_init(void) {
  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_GET_PAGESIZE,
    ptedit_command_get_page_size
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_GET_PAT,
    ptedit_command_get_pat
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_GET_ROOT,
     ptedit_command_get_root
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_INVALIDATE_TLB,
    ptedit_command_invalidate_tlb
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_READ_PAGE,
    ptedit_command_read_page
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_SET_PAT,
    ptedit_command_set_pat
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_SET_ROOT,
    ptedit_command_set_root
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_SWITCH_TLB_INVALIDATION,
    ptedit_command_switch_tlb_invalidation
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_VM_LOCK,
    ptedit_command_vm_lock
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_VM_RESOLVE,
    ptedit_command_vm_resolve
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_VM_UNLOCK,
    ptedit_command_vm_unlock
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_VM_UPDATE,
    ptedit_command_vm_update
  );

  ptedit_command_device_register_command(
    PTEDITOR_IOCTL_CMD_WRITE_PAGE,
    ptedit_command_write_page
  );

  /* ORDER MATTERS! */

  if (!ptedit_shared_initialize_symbols() ||
      !ptedit_arch_initialize_symbols() ||
      !ptedit_command_device_install() ||
      !ptedit_arch_initialize_constants()) {
    return -ENXIO;
  }

  if (!ptedit_arch_install_devmem_hook()) {
    pr_alert("Could not bypass /dev/mem restriction\n");
  } else {
    pr_info("/dev/mem is now superuser read-/writable\n");
  }
  
  ptedit_umem_device_install();
  pr_info("Loaded.\n");

  return 0;
}

static void __exit pteditor_exit(void) {
  ptedit_command_device_uninstall();
  ptedit_command_device_clear_commands();
  ptedit_arch_uninstall_devmem_hook();
  ptedit_umem_device_uninstall();

  pr_info("Removed.\n");
}

module_init(pteditor_init);
module_exit(pteditor_exit);
