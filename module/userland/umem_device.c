#include <linux/proc_fs.h>
#include "../shared/shared.h"
#include "userland.h"

/* ------------------------------ DEFINITIONS ------------------------------ */

enum _device_status {
  _DEVICE_STATUS_UNINITIALIZED = 0,
  _DEVICE_STATUS_INITIALIZED
};

#define _SYM_IMPL_LSEEK "memory_lseek"
#define _SYM_IMPL_MMAP  "mmap_mem"
#define _SYM_IMPL_READ  "read_mem"
#define _SYM_IMPL_WRITE "write_mem"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
  typedef struct proc_ops _proc_ops;
#else
  typedef struct file_operations _proc_ops;
#endif

static int _umem_open(struct inode *inode, struct file *filp);

static _proc_ops _umem_operations;
static enum _device_status _umem_status = _DEVICE_STATUS_UNINITIALIZED;

#define _RESOLVE_SYMBOL(dst, name) \
  if (!(dst = (void *) ptedit_shared_kallsyms_lookup_name(name))) { \
    pr_err("Cannot resolve symbol '%s'.\n", name); \
    return PTEDIT_STATUS_ERROR; \
  }

#define _PROC_UMEM_NAME "umem"
#define _PROC_UMEM_MODE 0666

/* ---------------------------- IMPLEMENTATIONS ---------------------------- */

ptedit_status_t ptedit_umem_device_install(void) {
  void *lseek_impl, *mmap_impl, *read_impl, *write_impl;

  if (_umem_status != _DEVICE_STATUS_UNINITIALIZED) {
    return PTEDIT_STATUS_SUCCESS;
  }

  /*
   * Since /proc/umem is the same as /dev/mem, but with out requiring root
   * permissions for access, we simply resolve the device-specific operations
   * for /dev/mem from the Kernel and use those for our new device.
   */

  _RESOLVE_SYMBOL(lseek_impl, _SYM_IMPL_LSEEK);
  _RESOLVE_SYMBOL(mmap_impl, _SYM_IMPL_MMAP);
  _RESOLVE_SYMBOL(read_impl, _SYM_IMPL_READ);
  _RESOLVE_SYMBOL(write_impl, _SYM_IMPL_WRITE);

  /* Build the device structure. */

  memset(&_umem_operations, 0, sizeof(_proc_ops));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
  _umem_operations.proc_lseek = lseek_impl;
  _umem_operations.proc_mmap = mmap_impl;
  _umem_operations.proc_open = _umem_open;
  _umem_operations.proc_read = read_impl;
  _umem_operations.proc_write = write_impl;
#else
  _umem_operations.llseek = lseek_impl;
  _umem_operations.mmap = mmap_impl;
  _umem_operations.open = _umem_open;
  _umem_operations.owner = THIS_MODULE;
  _umem_operations.read = read_impl;
  _umem_operations.write = write_impl;
#endif

  /* Create the device. */

  if (
    !proc_create(_PROC_UMEM_NAME, _PROC_UMEM_MODE, NULL, &_umem_operations)
  ) {
    pr_err("Cannot create device at '/proc/" _PROC_UMEM_NAME "'.\n");
    return PTEDIT_STATUS_ERROR;
  }

  _umem_status = _DEVICE_STATUS_INITIALIZED;
  pr_info(
    "Installed unprivileged memory access via device at '/proc/"
    _PROC_UMEM_NAME
    "'.\n"
  );

  return PTEDIT_STATUS_SUCCESS;
}

void ptedit_umem_device_uninstall(void) {
  if (_umem_status == _DEVICE_STATUS_UNINITIALIZED) {
    return;
  }

  remove_proc_entry(_PROC_UMEM_NAME, NULL);
  _umem_status = _DEVICE_STATUS_UNINITIALIZED;
  pr_info(
    "Uninstalled unprivileged memory access via device '/proc/"
    _PROC_UMEM_NAME
    "'.\n"
  );
}

static int _umem_open(struct inode *inode, struct file *filp) {
  return 0;
}
