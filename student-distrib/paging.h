/* paging.h: Header file for all paging implementations */
#ifndef _PAGING_H
#define _PAGING_H

/* This is a page directory entry for page table.  It goes in the Page Directory . */
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

/*this is a page directory entry for 4MB page entry. It goes in the Page Directory */
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

/*this is a page table entry for 4kB page entry. It goes in the Page Table */
typedef struct page_table_entry {
    union {
        uint32_t val;
        struct {
            uint32_t present : 1; //if 1, exist. 0 - not exist
            uint32_t wr : 1; // 1 - enable write and read. 0 - read only
            uint32_t us : 1; // 1 - user mode, 0 - sudo mode
            uint32_t write_through : 1; // 1 - write through in cache is enabled. 0 - write back
            uint32_t cached : 1; // 1 - cached disabled, 0 - cached enabled
            uint32_t accessed : 1; //1 - accessed . 0 - not
            uint32_t dirty : 1; //1 - been accessed before . 0 - not
            uint32_t pat : 1;
            uint32_t global : 1; //1 - global enabled . 0 - not
            uint32_t available : 3;
            uint32_t physical_page_addr : 20; //physical memory address for this 4kB page
        } __attribute__ ((packed));
    };
} page_table_entry_t;

void init_paging();

#endif  /* _PAGING_H */
