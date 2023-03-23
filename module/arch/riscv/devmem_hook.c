#include "../arch.h"

ptedit_status_t ptedit_arch_install_devmem_hook(void) {
  /*
   * Due to the lack of hardware to test this, we only support this feature on
   * Tina kernels for now.
   */

#if !PTEDIT_TINA_BUILD
  pr_warn(
    "ptedit_arch_install_devmem_hook is only supported with Tina Kernels on "
    "RISC-V!"
  );

  return PTEDIT_STATUS_ERROR;
#else
  return PTEDIT_STATUS_SUCCESS;
#endif
}

void ptedit_arch_uninstall_devmem_hook(void) {
}
