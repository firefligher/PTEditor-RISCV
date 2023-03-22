#pragma once

#define PTEDIT_PAGE_PRESENT 1

#pragma pack(push, 1)
  typedef struct {
    union {
      size_t valid : 1;
      size_t present : 1;
    };

    size_t readable : 1;
    size_t writable : 1;
    size_t executable : 1;
    size_t user_mode_accessible : 1;
    size_t global : 1;
    size_t accessed : 1;
    size_t dirty : 1;
    size_t rsw : 2;

    union {
      struct {
        size_t ppn_0 : 9;
        size_t ppn_1 : 9;
        size_t ppn_2 : 26;
      };

      size_t pfn : 44;
    };

    size_t reserved : 10;
  } ptedit_pte_sv39_t;
#pragma pack(pop)

#pragma pack(push, 1)
  typedef struct {
    size_t valid : 1;
    size_t readable : 1;
    size_t writable : 1;
    size_t executable : 1;
    size_t user_mode_accessible : 1;
    size_t global : 1;
    size_t accessed : 1;
    size_t dirty : 1;
    size_t rsw : 2;

    size_t ppn_0 : 9;
    size_t ppn_1 : 9;
    size_t ppn_2 : 9;
    size_t ppn_3 : 17;

    size_t reserved : 10;
  } ptedit_pte_sv48_t;
#pragma pack(pop)

typedef ptedit_pte_sv39_t ptedit_pgd_t;
typedef ptedit_pte_sv39_t ptedit_p4d_t;
typedef ptedit_pte_sv39_t ptedit_pud_t;
typedef ptedit_pte_sv39_t ptedit_pmd_t;
typedef ptedit_pte_sv39_t ptedit_pte_t;
