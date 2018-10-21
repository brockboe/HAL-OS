#include "types.h"
#include "paging-header.h"

#define PAGE_TABLE_LEN 1024
#define PAGE_DIR_LEN 1024
#define PAGE_SIZE 1024
#define PAGING_SHIFT 12

extern void init_control_reg();

extern uint32_t paging_table[PAGE_TABLE_LEN];
extern uint32_t directory_paging[PAGE_DIR_LEN];

void init_paging(){
          int video_offset_in_pt = 184;
          int i;

          page_directory_entry_4kb_t tmp;
          //add first entry in pd to be the page table
          tmp.present = 1;
          tmp.wr = 1;
          tmp.us = 1;
          tmp.write_through = 1;
          tmp.cached = 0;
          tmp.accessed = 0;
          tmp.paddling = 0;
          tmp.page_size = 0;
          tmp.g =0;
          tmp.available = 0;
          tmp.table_base_addr = ((uint32_t)paging_table >> PAGING_SHIFT);
          directory_paging[0] = (uint32_t) tmp.val;

         page_table_entry_t temp;

         temp.present = 1;
         temp.wr = 1;
         temp.us = 1;
         temp.write_through = 1;
         temp.cached = 0;
         temp.accessed = 0;
         temp.dirty = 0;
         temp.pat = 0;
         temp.global = 1;
         temp.available = 0;
         temp.physical_page_addr = (0x000B8);
         paging_table[0xB8] = (uint32_t)temp.val;


          page_directory_entry_4mb_t temp_kernel;
          temp_kernel.present = 1;
          temp_kernel.wr = 1;
          temp_kernel.us = 0;
          temp_kernel.write_through = 1;
          temp_kernel.cached = 1;
          temp_kernel.accessed = 0;
          temp_kernel.paddling = 0;
          temp_kernel.page_size = 1;
          temp_kernel.g = 0;
          temp_kernel.available = 0;
          temp_kernel.pat = 0;
          temp_kernel.reserved = 0;
          temp_kernel.page_base_addr = 1;
          directory_paging[1] = (uint32_t) temp_kernel.val;

          for(i = 2; i < PAGE_SIZE; i++){
              temp_kernel.present = 0;
              directory_paging[i] = (uint32_t) temp_kernel.val;
          }

          init_control_reg();

      return;
}