#pragma once

#ifdef __linux__
  #include <linux/types.h>
#endif

/*
 * Set up the common prefix for all messages that we print during the runtime
 * of this module and will be stored in the kernel log.
 */

#ifdef __linux__
  #ifdef pr_fmt
    #undef pr_fmt
  #endif

  #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#endif

/*
 * Define the paths for command device.
 */

#ifdef __linux__
  #define PTEDITOR_DEVICE_NAME "pteditor"
  #define PTEDITOR_DEVICE_PATH "/dev/" PTEDITOR_DEVICE_NAME
#else
  #define PTEDITOR_DEVICE_NAME L"PTEditorLink"
  #define PTEDITOR_DEVICE_PATH L"\\\\.\\" PTEDITOR_DEVICE_NAME
#endif

/*
 * Define the IOCTL stuff for the userspace command interface.
 */

#ifdef __linux__
  #define PTEDITOR_IOCTL_MAGIC_NUMBER ((long) 0x3d17)

  #define PTEDITOR_IOCTL_CMD_VM_RESOLVE \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 1, size_t)
#endif

/*
 * Description TBD.
 */

#define PTEDIT_VALID_MASK_PGD (1<<0)
#define PTEDIT_VALID_MASK_P4D (1<<1)
#define PTEDIT_VALID_MASK_PUD (1<<2)
#define PTEDIT_VALID_MASK_PMD (1<<3)
#define PTEDIT_VALID_MASK_PTE (1<<4)
