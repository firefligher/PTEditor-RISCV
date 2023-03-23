#pragma once

/*
 * Ensure that PTEDIT_MODULE_BUILD is defined.
 *
 * If PTEDIT_MODULE_BUILD does not resolve to zero, we build the Kernel module,
 * otherwise we build something else.
 */

#ifndef PTEDIT_MODULE_BUILD
  #define PTEDIT_MODULE_BUILD 0
#endif

/*
 * Ensure that PTEDIT_TINA_BUILD is defined.
 *
 * If PTEDIT_TINA_BUILD does not resolve to zero, we build the Kernel module
 * for a RISC-V Tina Kernel, otherwise we build something else.
 */

#ifndef PTEDIT_TINA_BUILD
  #define PTEDIT_TINA_BUILD 0
#endif

/*
 * Ensure that PTEDIT_T_HEAD_C9XX_BUILD is defined.
 *
 * If PTEDIT_T_HEAD_C9XX_BUILD does not resolve to zero, we build either the
 * Kernel module or the userland library for a T-HEAD C9xx chip (RISC-V).
 */

#ifndef PTEDIT_T_HEAD_C9XX_BUILD
  #define PTEDIT_T_HEAD_C9XX_BUILD 0
#endif

/*
 * Platform-specific macros.
 */

#define PTEDIT_PLATFORM_LINUX   0
#define PTEDIT_PLATFORM_WINDOWS 1

#ifdef __linux__
  #define PTEDIT_CURRENT_PLATFORM PTEDIT_PLATFORM_LINUX
#endif

#ifdef _WIN32
  #ifdef PTEDIT_CURRENT_PLATFORM
    #error Ambiguous platform macros present.
  #endif

  #define PTEDIT_CURRENT_PLATFORM PTEDIT_PLATFORM_WINDOWS
#endif

#ifndef PTEDIT_CURRENT_PLATFORM
  #error Unsupport platform or identification failed.
#endif

#define PTEDIT_ON_LINUX   (PTEDIT_CURRENT_PLATFORM == PTEDIT_PLATFORM_LINUX)
#define PTEDIT_ON_WINDOWS (PTEDIT_CURRENT_PLATFORM == PTEDIT_PLATFORM_WINDOWS)

/*
 * Sanity check:  Since a module can only be built for Linux, it must apply
 *                that, if PTEDIT_MODULE_BUILD is not zero, we must be on
 *                Linux.
 */

#if PTEDIT_MODULE_BUILD && !PTEDIT_ON_LINUX
  #error Invalid configuration! Not on Linux, but we do a module build!
#endif

/*
 * Sanity check:  We only support the T-Head C9xx on Linux.
 */

#if PTEDIT_T_HEAD_C9XX_BUILD && !PTEDIT_ON_LINUX
  #error Invalid configuration! Building for T-Head C9xx, but not on Linux \
apparently!
#endif

/*
 * Architecture-specific macros.
 */

#define PTEDIT_ARCH_X86   0
#define PTEDIT_ARCH_ARM   1
#define PTEDIT_ARCH_RISCV 2

#if defined(__i386__) || defined(__x86_64__)
  #define PTEDIT_CURRENT_ARCH PTEDIT_ARCH_X86
#endif

#ifdef __aarch64__
  #ifdef PTEDIT_CURRENT_ARCH
    #error Ambiguous architecture macros present.
  #endif

  #define PTEDIT_CURRENT_ARCH PTEDIT_ARCH_ARM
#endif

#ifdef __riscv
  #ifdef PTEDIT_CURRENT_ARCH
    #error Ambiguous architecture macros present.
  #endif

  #define PTEDIT_CURRENT_ARCH PTEDIT_ARCH_RISCV
#endif

#ifndef PTEDIT_CURRENT_ARCH
  #error Unsupport architecture or identification failed.
#endif

#define PTEDIT_ON_ARM   (PTEDIT_CURRENT_ARCH == PTEDIT_ARCH_ARM)
#define PTEDIT_ON_RISCV (PTEDIT_CURRENT_ARCH == PTEDIT_ARCH_RISCV)
#define PTEDIT_ON_X86   (PTEDIT_CURRENT_ARCH == PTEDIT_ARCH_X86)

/*
 * Sanity check:  A Tina build implies RISC-V and module build.
 */

#if PTEDIT_TINA_BUILD && (!PTEDIT_ON_RISCV || !PTEDIT_MODULE_BUILD)
  #error Invalid configuration! Tina build without RISC-V or module build.
#endif

/*
 * Includes that may be required somewhere in the following.
 */

#if PTEDIT_ON_LINUX
  #include <linux/types.h>
  #include <linux/version.h>
#endif

/*
 * Set up the common prefix for all messages that we print during the runtime
 * of this module and will be stored in the kernel log.
 */

#if PTEDIT_ON_LINUX && PTEDIT_MODULE_BUILD
  #ifdef pr_fmt
    #undef pr_fmt
  #endif

  #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#endif

/*
 * Define the paths for command device.
 */

#if PTEDIT_ON_LINUX
  #define PTEDITOR_DEVICE_NAME "pteditor"
  #define PTEDITOR_DEVICE_PATH "/dev/" PTEDITOR_DEVICE_NAME
#endif

#if PTEDIT_ON_WINDOWS
  #define PTEDITOR_DEVICE_NAME L"PTEditorLink"
  #define PTEDITOR_DEVICE_PATH L"\\\\.\\" PTEDITOR_DEVICE_NAME
#endif

/*
 * Define the IOCTL stuff for the userspace command interface.
 */

#ifdef PTEDIT_ON_LINUX
  #define PTEDITOR_IOCTL_MAGIC_NUMBER ((long) 0x3d17)

  #define PTEDITOR_IOCTL_CMD_VM_RESOLVE \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 1, size_t)

  #define PTEDITOR_IOCTL_CMD_VM_UPDATE \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 2, size_t)

  #define PTEDITOR_IOCTL_CMD_VM_LOCK \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 3, size_t)

  #define PTEDITOR_IOCTL_CMD_VM_UNLOCK \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 4, size_t)

  #define PTEDITOR_IOCTL_CMD_READ_PAGE \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 5, size_t)

  #define PTEDITOR_IOCTL_CMD_WRITE_PAGE \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 6, size_t)

  #define PTEDITOR_IOCTL_CMD_GET_ROOT \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 7, size_t)

  #define PTEDITOR_IOCTL_CMD_SET_ROOT \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 8, size_t)

  #define PTEDITOR_IOCTL_CMD_GET_PAGESIZE \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 9, size_t)

  #define PTEDITOR_IOCTL_CMD_INVALIDATE_TLB \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 10, size_t)

  #define PTEDITOR_IOCTL_CMD_GET_PAT \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 11, size_t)

  #define PTEDITOR_IOCTL_CMD_SET_PAT \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 12, size_t)

  #define PTEDITOR_IOCTL_CMD_SWITCH_TLB_INVALIDATION \
    _IOR(PTEDITOR_IOCTL_MAGIC_NUMBER, 13, size_t)
#endif

/*
 * Detect, if the current platform supports five levels of page tables.
 */

#if PTEDIT_MODULE_BUILD
  #define PTEDIT_HAS_P4D  (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
#else
  /*
   * NOTE:  Windows may provide equivalent support, but it is not implemented.
   *        That's left for the reader.
   */

  #define PTEDIT_HAS_P4D  0
#endif
