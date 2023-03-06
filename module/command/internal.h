#pragma once

#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
  #define from_user copy_from_user
  #define to_user copy_to_user
#else
  #define from_user raw_copy_from_user
  #define to_user raw_copy_to_user
#endif

static inline pid_t sanitize_pid(pid_t pid) {
  return (pid)
    ? pid
    : current->pid;
}
