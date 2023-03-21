#include <linux/compiler.h>
#include <linux/current.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/smp.h>
#include "../arch.h"

typedef struct tlb_page_s {
  struct vm_area_struct* vma;
  unsigned long addr;
} tlb_page_t;

void _flush_tlb_page_smp(void* info) {
  tlb_page_t* tlb_page = (tlb_page_t*) info;
  flush_tlb_page(tlb_page->vma, tlb_page->addr);
}

void ptedit_arch_invalidate_tlb_kernel(void *addr) {
  struct vm_area_struct *vma = find_vma(current->mm, (unsigned long) addr);
  tlb_page_t tlb_page;
  if (unlikely(vma == NULL || addr < vma->vm_start)) {
    return;
  }
  tlb_page.vma = vma;
  tlb_page.addr = (unsigned long) addr;
  on_each_cpu(_flush_tlb_page_smp, &tlb_page, 1);
}
