#include <linux/compiler.h>
#include <linux/fs.h>
#include <linux/kprobes.h>
#include <linux/printk.h>
#include <linux/slab.h>

#include "../arch/arch.h"
#include "shared.h"

#define PATH_KALLSYMS_FILE        "/proc/kallsyms"
#define SYM_KALLSYMS_LOOKUP_NAME  "kallsyms_lookup_name"
#define SYM_PTEDIT_FIXED_POINT    "ptedit_shared_fixed_point"
#define BUFFER_SIZE               4096

unsigned long (*ptedit_shared_kallsyms_lookup_name)(const char *name);
void (*ptedit_shared_invalidate_tlb)(unsigned long);

static int resolve_kallsyms_lookup_name_with_kprobe(void);
static int resolve_kallsyms_lookup_name_with_fs(void);
void ptedit_shared_fixed_point(void);

int ptedit_shared_initialize_symbols(void) {
  ptedit_shared_invalidate_tlb = ptedit_arch_invalidate_tlb_kernel;

  /*
   * Task:        We want to resolve the address of the kallsyms_lookup_name
   *              function, which we will need to resolve other (unexported)
   *              symbols in the kernel.
   *
   * Issue:       The kallsyms_lookup_name function itself may not be exported
   *              and we cannot use it directly.
   *
   * Solution 1:  We try to use a kernel probe to resolve its address. Kernel
   *              probes may not be available everywhere.
   *
   * Solution 2:  We try to resolve its address via '/proc/kallsyms'. In some
   *              cases this actually contains the address that we are looking
   *              for.
   *              For the sake of verification, we first verify, if the
   *              addresses, that are outputed by '/proc/kallsyms', match our
   *              own address space.
   *              For this purpose, we use a module-internal function as fixed
   *              point.
   */

  return resolve_kallsyms_lookup_name_with_kprobe()
      || resolve_kallsyms_lookup_name_with_fs();
}

static int resolve_kallsyms_lookup_name_with_kprobe(void) {
  struct kprobe probe = {
    .symbol_name = SYM_KALLSYMS_LOOKUP_NAME
  };

  pr_info(
    "Attempting to resolve '"
    SYM_KALLSYMS_LOOKUP_NAME
    "' with kprobe.\n"
  );

  if (register_kprobe(&probe)) {
    pr_info("Registering kprobe failed.\n");
    return 0;
  }

  ptedit_shared_kallsyms_lookup_name = (void *) probe.addr;
  unregister_kprobe(&probe);

  if (!ptedit_shared_kallsyms_lookup_name) {
    pr_warn(
      "Kprobe was unable to resolve '"
      SYM_KALLSYMS_LOOKUP_NAME
      "'.\n"
    );

    return 0;
  }

  return 1;
}

static int resolve_kallsyms_lookup_name_with_fs(void) {
  /*
   * Adapted from https://stackoverflow.com/a/1184346 and
   * https://stackoverflow.com/a/53917617.
   */

  struct file *kallsyms_file;
  unsigned long long int addr_fixed_point = 0, addr_kallsyms_lookup_name = 0;
  char *buf;
  size_t buf_offset = 0;
  loff_t file_offset = 0;

  kallsyms_file = filp_open(PATH_KALLSYMS_FILE, O_RDONLY, 0);

  if (!kallsyms_file || IS_ERR(kallsyms_file)) {
    pr_warn("Cannot open file at '" PATH_KALLSYMS_FILE "'.\n");
    return 0;
  }

  /*
   * Since maximum stack frame sizes and the defined value for BUFFER_SIZE may
   * change over time, heap-allocated memory is safer.
   */

  buf = kmalloc(BUFFER_SIZE + 1, GFP_KERNEL);

  do {
    size_t buf_limit, buf_newline_offset;
    ssize_t buf_read;
    char *buf_newline;
    int skip_one_line_only = 0;

    buf_read = kernel_read(
      kallsyms_file,
      buf + buf_offset,
      BUFFER_SIZE - buf_offset,
      &file_offset
    );

    if (buf_read < 1) {
      break;
    }

    /* Important: Place the trailing zero byte. */

    buf_limit = buf_offset + buf_read;
    buf[buf_limit] = 0;

    /* Scan for the symbols and store their addresses. */

    if (
      (!addr_kallsyms_lookup_name && strstr(buf, SYM_KALLSYMS_LOOKUP_NAME)) ||
      (!addr_fixed_point && strstr(buf, SYM_PTEDIT_FIXED_POINT))
    ) {
      char sym_name[26], sym_type;
      unsigned long long int sym_addr;

      skip_one_line_only = 1;

      if (sscanf(buf, "%llx %c %25s\n", &sym_addr, &sym_type, sym_name) == 3) {
        if (
          sym_type == 'T' &&
          strcmp(SYM_KALLSYMS_LOOKUP_NAME, sym_name) == 0
        ) {
          addr_kallsyms_lookup_name = sym_addr;
        }

        if (sym_type == 't' && strcmp(SYM_PTEDIT_FIXED_POINT, sym_name) == 0) {
          addr_fixed_point = sym_addr;
        }
      }
    }

    /* Remove all complete lines and continue. */

    buf_newline = skip_one_line_only
      ? strchr(buf, '\n')
      : strrchr(buf, '\n');

    if (!buf_newline) {
      pr_info("No newline!");
      break;
    }

    buf_newline_offset = buf_newline - buf;

    /*
     * The off-by-one copy is intended. We do not want to copy the newline
     * character. We also do not copy the trailing zero, because we do not
     * perform any string operations before we fix this within the next
     * iteration.
     */

    memmove(
      buf,
      buf + buf_newline_offset + 1,
      buf_limit - buf_newline_offset - 1
    );

    buf_offset = buf_limit - buf_newline_offset - 1;
  } while (!addr_fixed_point || !addr_kallsyms_lookup_name);

  kfree(buf);
  filp_close(kallsyms_file, 0);

  if (!addr_fixed_point || !addr_kallsyms_lookup_name) {
    pr_warn(
      "Failed to identify symbols. ("
      SYM_PTEDIT_FIXED_POINT
      ": %llx, "
      SYM_KALLSYMS_LOOKUP_NAME
      ": %llx)\n",
      addr_fixed_point,
      addr_kallsyms_lookup_name
    );

    return 0;
  }

  pr_info(
    "Identified symbols. ("
    SYM_PTEDIT_FIXED_POINT
    ": %llx, "
    SYM_KALLSYMS_LOOKUP_NAME
    ": %llx)\n",
    addr_fixed_point,
    addr_kallsyms_lookup_name
  );

  /* Sanity check. */

  if (
    addr_fixed_point != ((unsigned long long int) ptedit_shared_fixed_point)
  ) {
    pr_warn(
      "Sanity check with fixed point failed!\n"
      "(Resolved: %llx, Actual: %llx)\n.",
      addr_fixed_point,
      (unsigned long long int) ptedit_shared_fixed_point
    );

    return 0;
  }

  ptedit_shared_kallsyms_lookup_name = (void *) addr_fixed_point;
  return 1;
}

/*
 * The following function only exists for the purpose of having a fixed point
 * for reflection-like access. Changing the name or removing the function may
 * result in hard-to-debug issues.
 */

void ptedit_shared_fixed_point(void) {
  /* TODO: Add some ASCII art */

  pr_warn("Do not call this function!\n");
}
