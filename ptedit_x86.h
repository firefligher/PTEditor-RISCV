#pragma once

 /** Page is present */
#define PTEDIT_PAGE_BIT_PRESENT 0
/** Page is writeable */
#define PTEDIT_PAGE_BIT_RW 1
/** Page is userspace addressable */
#define PTEDIT_PAGE_BIT_USER 2
/** Page write through */
#define PTEDIT_PAGE_BIT_PWT 3
/** Page cache disabled */
#define PTEDIT_PAGE_BIT_PCD 4
/** Page was accessed (raised by CPU) */
#define PTEDIT_PAGE_BIT_ACCESSED 5
/** Page was written to (raised by CPU) */
#define PTEDIT_PAGE_BIT_DIRTY 6
/** 4 MB (or 2MB) page */
#define PTEDIT_PAGE_BIT_PSE 7
/** PAT (only on 4KB pages) */
#define PTEDIT_PAGE_BIT_PAT 7
/** Global TLB entry PPro+ */
#define PTEDIT_PAGE_BIT_GLOBAL 8
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW1 9
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW2 10
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW3 11
/** PAT (on 2MB or 1GB pages) */
#define PTEDIT_PAGE_BIT_PAT_LARGE 12
/** Available for programmer */
#define PTEDIT_PAGE_BIT_SOFTW4 58
/** Protection Keys, bit 1/4 */
#define PTEDIT_PAGE_BIT_PKEY_BIT0 59
/** Protection Keys, bit 2/4 */
#define PTEDIT_PAGE_BIT_PKEY_BIT1 60
/** Protection Keys, bit 3/4 */
#define PTEDIT_PAGE_BIT_PKEY_BIT2 61
/** Protection Keys, bit 4/4 */
#define PTEDIT_PAGE_BIT_PKEY_BIT3 62
/** No execute: only valid after cpuid check */
#define PTEDIT_PAGE_BIT_NX 63

/** Strong uncachable (nothing is cached) */
#define PTEDIT_MT_UC      0
/** Write combining (consecuite writes are combined in a WC buffer and then written once) */
#define PTEDIT_MT_WC      1
/** Write through (read accesses are cached, write access are written to cache and memory) */
#define PTEDIT_MT_WT      4
/** Write protected (only read access is cached) */
#define PTEDIT_MT_WP      5
/** Write back (read and write accesses are cached) */
#define PTEDIT_MT_WB      6
/** Uncachable (as UC, but can be changed to WC through MTRRs) */
#define PTEDIT_MT_UCMINUS 7

#define PTEDIT_PAGE_PRESENT 1

/**
 * Struct to access the fields of the PGD
 */
#pragma pack(push,1)
typedef struct {
    size_t present : 1;
    size_t writeable : 1;
    size_t user_access : 1;
    size_t write_through : 1;
    size_t cache_disabled : 1;
    size_t accessed : 1;
    size_t ignored_3 : 1;
    size_t size : 1;
    size_t ignored_2 : 4;
    size_t pfn : 28;
    size_t reserved_1 : 12;
    size_t ignored_1 : 11;
    size_t execution_disabled : 1;
} ptedit_pgd_t;
#pragma pack(pop)


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
 * Struct to access the fields of the PMD when mapping a  large page (2MB)
 */
#pragma pack(push,1)
typedef struct {
    size_t present : 1;
    size_t writeable : 1;
    size_t user_access : 1;
    size_t write_through : 1;
    size_t cache_disabled : 1;
    size_t accessed : 1;
    size_t dirty : 1;
    size_t size : 1;
    size_t global : 1;
    size_t ignored_2 : 3;
    size_t pat : 1;
    size_t reserved_2 : 8;
    size_t pfn : 19;
    size_t reserved_1 : 12;
    size_t ignored_1 : 11;
    size_t execution_disabled : 1;
} ptedit_pmd_large_t;
#pragma pack(pop)

/**
 * Struct to access the fields of the PTE
 */
#pragma pack(push,1)
typedef struct {
    size_t present : 1;
    size_t writeable : 1;
    size_t user_access : 1;
    size_t write_through : 1;
    size_t cache_disabled : 1;
    size_t accessed : 1;
    size_t dirty : 1;
    size_t size : 1;
    size_t global : 1;
    size_t ignored_2 : 3;
    size_t pfn : 28;
    size_t reserved_1 : 12;
    size_t ignored_1 : 11;
    size_t execution_disabled : 1;
} ptedit_pte_t;
#pragma pack(pop)
