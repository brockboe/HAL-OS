#include "types.h"
#include "paging.h"

extern void init_control_reg(uint32_t * CR3);


/* init_paging function :
 * DESCRIPTION:  the funcion is being called to enable paging ,
 *               it set up the page directory with only one page table entry and one 4MB page entry of kernel
 * INPUT : none
 * OUTPUT : none
 * PRELIMINARY :
 *             video memory is 4kB at 0xB8000, which is 0000 0000 00 | 00 1011 1000 | 0000 0000 0000
 *                                                  directory #  |  table #     | page offset
 *             thus, the table where pointer to the video mem should be at index 0 of PD
 *             and the pointer to there the video mem is should be at index 0xB8 of PT
 *  -------------------------------------------------------------------------------------------------
 *             kernel memory is 4 MB at 0x400000, which is  0000 0000 01| 00 0000 0000 | 0000 0000 0000
 *             thus it should be at index 1 on page directory ,
 *             with us set to 0
*/
void init_paging(){

        // set the first entry in PD pointing to the PT,
          page_directory_entry_4kb_t tmp;
        //   to set paging present to be 1,
        //   enable write and read, set the page to public,
        //   enable write through
        //   enable cache
        //   set accessed to be 0 at the beginning
        //   page size is 4kB
        //   set ignore bit to be 0
        //   set available 3 bits to be 0 since we dont care
        //   set the paging base address to be the table we decleared earlier
          tmp.present = 1;
          tmp.wr = 1;
          tmp.us = 0;
          tmp.write_through = 1;
          tmp.cached = 0;
          tmp.accessed = 0;
          tmp.paddling = 0;
          tmp.page_size = 0;
          tmp.g = 1;
          tmp.available = 0;
          tmp.table_base_addr = ((uint32_t)paging_table >> PAGING_SHIFT);
          directory_paging[0] = (uint32_t) tmp.val; //add entry to the pd.

        //set the 0xB8 entry in the PT pointing to the physical video memory
         page_table_entry_t temp;
        //   to set paging present to be 1,
        //   enable write and read, set the page to public,
        //   enable write through
        //   enable cache
        //   set accessed to be 0 at the beginning
        //   page size is 4kB
        //   set ignore bit to be 0
        //   set available 3 bits to be 0 since we dont care
        //   set the paging base address to be the video memory 0xB8
         temp.present = 1;
         temp.wr = 1;
         temp.us = 0;
         temp.write_through = 1;
         temp.cached = 0;
         temp.accessed = 0;
         temp.dirty = 0;
         temp.pat = 0;
         temp.global = 1;
         temp.available = 0;
         temp.physical_page_addr = (0x000B8);
         paging_table[0xB8] = (uint32_t)temp.val;

        // set the second entry of PD pointing to the kernel space
          page_directory_entry_4mb_t temp_kernel;
        //   to set paging present to be 1,
        //   enable write and read, set the page to public,
        //   enable write through
        //   enable cache
        //   set accessed to be 0 at the beginning
        //   page size is 4kB
        //   set ignore bit to be 0
        //   set available 3 bits to be 0 since we dont care
        //   set the paging base address to be kernel address
          temp_kernel.present = 1;
          temp_kernel.wr = 1;
          temp_kernel.us = 0;
          temp_kernel.write_through = 1;
          temp_kernel.cached = 1;
          temp_kernel.accessed = 0;
          temp_kernel.paddling = 0;
          temp_kernel.page_size = 1;
          temp_kernel.g = 1;
          temp_kernel.available = 0;
          temp_kernel.pat = 0;
          temp_kernel.reserved = 0;
          temp_kernel.page_base_addr = 1;
          directory_paging[1] = (uint32_t) temp_kernel.val;

          init_control_reg(directory_paging);

      return;
}
