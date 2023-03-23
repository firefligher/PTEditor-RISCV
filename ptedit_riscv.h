#pragma once

#include "config.h"

#define PTEDIT_PAGE_BIT_VALID                 0
#define PTEDIT_PAGE_BIT_READABLE              1
#define PTEDIT_PAGE_BIT_WRITABLE              2
#define PTEDIT_PAGE_BIT_EXECUTABLE            3
#define PTEDIT_PAGE_BIT_USER_MODE_ACCESSIBLE  4
#define PTEDIT_PAGE_BIT_GLOBAL                5
#define PTEDIT_PAGE_BIT_ACCESSED              6
#define PTEDIT_PAGE_BIT_DIRTY                 7

#define PTEDIT_PAGE_BIT_T_HEAD_C9XX_SHAREABLE     60
#define PTEDIT_PAGE_BIT_T_HEAD_C9XX_BUFFERABLE    61
#define PTEDIT_PAGE_BIT_T_HEAD_C9XX_CACHEABLE     62
#define PTEDIT_PAGE_BIT_T_HEAD_C9XX_STRONG_ORDER  63

#define PTEDIT_PAGE_PRESENT 1

#define PTEDIT_MT_T_HEAD_C9XX_SHAREABLE     \
  PTEDIT_PAGE_BIT_T_HEAD_C9XX_SHAREABLE

#define PTEDIT_MT_T_HEAD_C9XX_BUFFERABLE    \
  PTEDIT_PAGE_BIT_T_HEAD_C9XX_BUFFERABLE

#define PTEDIT_MT_T_HEAD_C9XX_CACHEABLE     \
  PTEDIT_PAGE_BIT_T_HEAD_C9XX_CACHEABLE

#define PTEDIT_MT_T_HEAD_C9XX_STRONG_ORDER  \
  PTEDIT_PAGE_BIT_T_HEAD_C9XX_STRONG_ORDER

#pragma pack(push, 1)
  typedef struct {
    size_t present : 1;
    size_t readable : 1;
    size_t writable : 1;
    size_t executable : 1;
    size_t user_mode_accessible : 1;
    size_t global : 1;
    size_t accessed : 1;
    size_t dirty : 1;
    size_t rsw : 2;
    size_t pfn : 44;

    size_t reserved : 10;
  } ptedit_pte_sv39_t;
#pragma pack(pop)

#pragma pack(push, 1)
  typedef struct {
    size_t present : 1;
    size_t readable : 1;
    size_t writable : 1;
    size_t executable : 1;
    size_t user_mode_accessible : 1;
    size_t global : 1;
    size_t accessed : 1;
    size_t dirty : 1;
    size_t rsw : 2;
    size_t pfn : 44;

    size_t reserved : 10;
  } ptedit_pte_sv48_t;
#pragma pack(pop)

typedef ptedit_pte_sv39_t ptedit_pgd_t;
typedef ptedit_pte_sv39_t ptedit_p4d_t;
typedef ptedit_pte_sv39_t ptedit_pud_t;
typedef ptedit_pte_sv39_t ptedit_pmd_t;
typedef ptedit_pte_sv39_t ptedit_pte_t;
