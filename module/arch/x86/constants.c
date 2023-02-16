#include "../arch.h"

int real_page_size = 4096;
int real_page_shift = 12;

int ptedit_arch_initialize_constants(void) {
  return 1;
}
