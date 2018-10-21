/* paging.h: Header file for all paging implementations */
#ifndef _PAGING_H
#define _PAGING_H

#define PAGE_SHIFT 12 //shift to get the actual physical address offset
#define PAGE_SIZE 1024

/* TODO: Implement paging initialization function header */

/* This is a page directory entry .  It goes in the Page Directory . */
typedef struct page_directory_entry_4kb  {
    union {
        uint32_t val;
        struct {
            uint32_t present : 1; //if 1, present at physical location
            uint32_t wr : 1; //if 1, read and write. 0 read only
            uint32_t us : 1; //user / supervisor mode, 1 - public.  0 - sudo
            uint32_t write_through : 1; //1 write through in cache is enabled . 0 - write back
            uint32_t cached : 1; //1 - not cached. 0 - cached
            uint32_t accessed : 1; //1 - accessed . 0 -not
            uint32_t paddling : 1; //just a 0 bit fixed
            uint32_t page_size : 1; //1 - 4MB page 0- 4kB page
            uint32_t g : 1; //just a padding
            uint32_t available : 3; //3 bits 9:11 could be used later to store more info
            uint32_t table_base_addr : 20; //corresponding pde table location
        } __attribute__ ((packed));
    };
} page_directory_entry_4kb_t;

typedef struct page_directory_entry_4mb  {
    union {
        uint32_t val;
        struct {
            uint32_t present : 1; //if 1, present at physical location
            uint32_t wr : 1; //if 1, read and write. 0 read only
            uint32_t us : 1; //user / supervisor mode, 1 - public.  0 - sudo
            uint32_t write_through : 1; //1 write through in cache is enabled . 0 - write back
            uint32_t cached : 1; //1 - not cached. 0 - cached
            uint32_t accessed : 1; //1 - accessed . 0 -not
            uint32_t paddling : 1; //just a 0 bit fixed
            uint32_t page_size : 1; //1 - 4MB page 0- 4kB page
            uint32_t g : 1; //just a padding
            uint32_t available : 3; //3 bits 9:11 could be used later to store more info
            uint32_t pat : 1;
            uint32_t reserved : 9;
            uint32_t page_base_addr : 10; //corresponding page address
        } __attribute__ ((packed));
    };
} page_directory_entry_4mb_t;

typedef struct page_table_entry {
    union {
        uint32_t val;
        struct {
            uint32_t present : 1;
            uint32_t wr : 1;
            uint32_t us : 1;
            uint32_t write_through : 1;
            uint32_t cached : 1;
            uint32_t accessed : 1;
            uint32_t dirty : 1;
            uint32_t pat : 1;
            uint32_t global : 1;
            uint32_t available : 3;
            uint32_t physical_page_addr : 20;
        } __attribute__ ((packed));
    };
} page_table_entry_t;

void init_paging();

#endif  /* _PAGING_H */