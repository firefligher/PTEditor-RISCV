#pragma once

/** Entry type 1/2 */
#define PTEDIT_PAGE_BIT_TYPE_BIT0 0
/** Entry type 1/2 */
#define PTEDIT_PAGE_BIT_TYPE_BIT1 1
/** Memory attribute index 1/3 */
#define PTEDIT_PAGE_BIT_MAIR_BIT0 2
/** Memory attribute index 2/3 */
#define PTEDIT_PAGE_BIT_MAIR_BIT1 3
/** Memory attribute index 3/3 */
#define PTEDIT_PAGE_BIT_MAIR_BIT2 4
/** Page is non-secure */
#define PTEDIT_PAGE_BIT_NON_SECURE 5
/** Page permissions 1/2 */
#define PTEDIT_PAGE_BIT_PERMISSION_BIT0 6
/** Page permissions 2/2 */
#define PTEDIT_PAGE_BIT_PERMISSION_BIT1 7
/** Shareability domain 1/2 */
#define PTEDIT_PAGE_BIT_SHARE_BIT0 8
/** Shareability domain 2/2 */
#define PTEDIT_PAGE_BIT_SHARE_BIT1 9
/** Page was accessed (raised by CPU) */
#define PTEDIT_PAGE_BIT_ACCESSED 10
/** Page is not global */
#define PTEDIT_PAGE_BIT_NOT_GLOBAL 11
/** Contiguous */
#define PTEDIT_PAGE_BIT_CONTIGUOUS 52
/** Privileged execute never */
#define PTEDIT_PAGE_BIT_PXN 53
/** Execute never */
#define PTEDIT_PAGE_BIT_XN 54
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW1 55
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW2 56
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW3 57
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW4 58
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW5 59
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW6 60
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW7 61
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW8 62
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW9 63

 /** Strong uncachable (nothing is cached) */
#define PTEDIT_MT_UC      0x44
/** Write through (read accesses are cached, write access are written to cache and memory) */
#define PTEDIT_MT_WT      0xbb
/** Write back (read and write accesses are cached) */
#define PTEDIT_MT_WB      0xff

#define PTEDIT_PAGE_PRESENT 3


/**
 * Struct to access the fields of the PGD
 */
typedef struct {
    size_t present : 2;
    size_t ignored_1 : 10;
    size_t pfn : 36;
    size_t reserved : 4;
    size_t ignored_2 : 7;
    size_t pxn_table : 1;
    size_t xn_table : 1;
    size_t ap_table : 2;
    size_t ns_table : 1;
}__attribute__((__packed__)) ptedit_pgd_t;


/**
 * Struct to access the fields of the P4D
 */
typedef ptedit_pgd_t ptedit_p4d_t;


/**
 * Struct to access the fields of the PUD
 */
typedef ptedit_pgd_t ptedit_pud_t;


/**
 * Struct to access the fields of the PMD
 */
typedef ptedit_pgd_t ptedit_pmd_t;


/**
 * Struct to access the fields of the PGD when mapping a large page
 */
typedef struct {
    size_t present : 2;
    size_t memory_attributes_index : 3;
    size_t non_secure : 1;
    size_t access_permissions : 2;
    size_t shareability_field : 2;
    size_t access_flag : 1;
    size_t not_global : 1;
    size_t reserved_1 : 18;
    size_t pfn : 18;
    size_t reserved_2 : 4;
    size_t contiguous : 1;
    size_t privileged_execute_never : 1;
    size_t execute_never : 1;
    size_t ingored_1 : 4;
    size_t ignored_2 : 5;
}__attribute__((__packed__)) ptedit_pgd_large_t;


/**
 * Struct to access the fields of the PMD when mapping a large page
 */
typedef struct {
    size_t present : 2;
    size_t memory_attributes_index : 3;
    size_t non_secure : 1;
    size_t access_permissions : 2;
    size_t shareability_field : 2;
    size_t access_flag : 1;
    size_t not_global : 1;
    size_t reserved_1 : 9;
    size_t pfn : 27;
    size_t reserved_2 : 4;
    size_t contiguous : 1;
    size_t privileged_execute_never : 1;
    size_t execute_never : 1;
    size_t ingored_1 : 4;
    size_t ignored_2 : 5;
}__attribute__((__packed__)) ptedit_pmd_large_t;


/**
 * Struct to access the fields of the PTE
 */
typedef struct {
    size_t present : 2;
    size_t memory_attributes_index : 3;
    size_t non_secure : 1;
    size_t access_permissions : 2;
    size_t shareability_field : 2;
    size_t access_flag : 1;
    size_t not_global : 1;
    size_t pfn : 36;
    size_t reserved_1 : 4;
    size_t contiguous : 1;
    size_t privileged_execute_never : 1;
    size_t execute_never : 1;
    size_t ingored_1 : 4;
    size_t ignored_2 : 5;
}__attribute__((__packed__)) ptedit_pte_t;
