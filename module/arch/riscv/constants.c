#include "../arch.h"

int real_page_size = 4096;

ptedit_status_t ptedit_arch_initialize_constants(void) {
  /*
   * There are no constants that we would need to initialize here.
   */

  return PTEDIT_STATUS_SUCCESS;
}
