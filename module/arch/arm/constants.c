#include "../arch.h"

int real_page_size;
int real_page_shift;

int ptedit_arch_initialize_constants(void) {
  uint64_t tcr_el1;

  asm volatile("mrs %0, tcr_el1" : "=r" (tcr_el1));

  switch((tcr_el1 >> 14) & 3) {
      case 1:
          // 64k pages
          real_page_size = 64 * 1024;
          real_page_shift = 16;
          break;
      case 2:
          // 16k pages
          real_page_size = 16 * 1024;
          real_page_shift = 14;
          break;
      default:
          break;
  }
}