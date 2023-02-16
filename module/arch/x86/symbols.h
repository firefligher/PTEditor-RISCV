#pragma once

extern const void (*ptedit_arch_flush_tlb_mm_range)(
  struct mm_struct*,
  unsigned long,
  unsigned long,
  unsigned int,
  bool
);

extern const void (*ptedit_arch_write_cr4)(unsigned long);
