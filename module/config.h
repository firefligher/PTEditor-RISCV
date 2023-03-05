#pragma once

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
