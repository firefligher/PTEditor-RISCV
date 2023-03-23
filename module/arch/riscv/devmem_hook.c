#include "../arch.h"

int ptedit_arch_install_devmem_hook(void) {
  /*
   * Due to the lack of hardware to test this, we only support this feature on
   * Tina kernels for now.
   */

#if !PTEDIT_TINA_BUILD
  pr_warn(
    "ptedit_arch_install_devmem_hook is only supported with Tina Kernels on "
    "RISC-V!"
  );

  return 0;
#else
  return 1;
#endif
}

void ptedit_arch_uninstall_devmem_hook(void) {
}
