#pragma once

/*
 * Set up the common prefix for all messages that we print during the runtime
 * of this module and will be stored in the kernel log.
 */

#ifdef pr_fmt
    #undef pr_fmt
#endif

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
