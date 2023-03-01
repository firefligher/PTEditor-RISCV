#include <linux/compiler.h>
#include <linux/fs.h>
#include <linux/kprobes.h>
#include <linux/printk.h>

#include "../arch/arch.h"
#include "shared.h"

#define PATH_KALLSYMS_FILE        "/proc/kallsyms"
#define SYM_KALLSYMS_LOOKUP_NAME  "kallsyms_lookup_name"
#define SYM_PTEDIT_FIXED_POINT    "ptedit_shared_fixed_point"
#define BUFFER_SIZE               256

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
   * Adapted from https://stackoverflow.com/a/1184346.
   * According to https://stackoverflow.com/a/53917617, this does not work with
   * never kernels anymore and a slightly adjusted variant of this method has
   * to be used.
   */

  struct file *kallsyms_file;
  unsigned long long int addr_fixed_point, addr_kallsyms_lookup_name;
  char buf[BUFFER_SIZE + 1];
  size_t buf_offset;
  loff_t file_offset;

  kallsyms_file = filp_open(PATH_KALLSYMS_FILE, O_RDONLY, 0);

  if (!kallsyms_file || IS_ERR(kallsyms_file)) {
    pr_warn("Cannot open file at '" PATH_KALLSYMS_FILE "'.\n");
    return 0;
  }

  file_offset = 0;
  addr_fixed_point = 0;
  addr_kallsyms_lookup_name = 0;

  do {
    size_t buf_limit, buf_newline_offset;
    ssize_t buf_read;
    unsigned long long int sym_addr;
    char *buf_newline, sym_name[26];

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
      sscanf(buf, "%llx T %20s\n", &sym_addr, sym_name) == 2 ||
      sscanf(buf, "%llx t %25s\n", &sym_addr, sym_name) == 2
    ) {
      if (strcmp(SYM_KALLSYMS_LOOKUP_NAME, sym_name) == 0) {
        addr_kallsyms_lookup_name = sym_addr;
      }

      if (strcmp(SYM_PTEDIT_FIXED_POINT, sym_name) == 0) {
        addr_fixed_point = sym_addr;
      }
    }

    /* Remove one line and continue. */

    buf_newline = strchr(buf, '\n');

    if (!buf_newline) {
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

  filp_close(kallsyms_file, 0);

  if (!addr_fixed_point || addr_kallsyms_lookup_name) {
    pr_warn("Failed to identify symbols.\n");
    return 0;
  }

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
