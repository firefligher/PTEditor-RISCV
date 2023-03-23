#include "../arch.h"

size_t ptedit_arch_get_pat(void) {
  pr_alert("ptedit_arch_get_pat is not supported on RISC-V.\n");
  return 0;
}

void ptedit_arch_set_pat(size_t pat) {
  pr_alert("ptedit_arch_set_pat is not supported on RISC-V.\n");
}
