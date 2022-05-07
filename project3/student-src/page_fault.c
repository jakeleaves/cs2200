#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table
   vpn_t vpn = vaddr_vpn(addr);
   pte_t* page_table = (pte_t*) (mem + PTBR * PAGE_SIZE);
   pte_t* ptFault_entry = &page_table[vpn];


   pfn_t pFault_pfn = free_frame();

   ptFault_entry -> pfn = pFault_pfn;
   ptFault_entry -> valid = 1;
   ptFault_entry -> dirty = 0;
  

   fte_t* ftable_entry = (fte_t*) (frame_table + pFault_pfn); 
   ftable_entry -> mapped = 1;
   ftable_entry -> process = current_process;
   ftable_entry -> vpn = vpn;


    if (swap_exists(ptFault_entry)) {
       swap_read(ptFault_entry, &mem[pFault_pfn * PAGE_SIZE]);     
    } else {
       memset(&mem[pFault_pfn * PAGE_SIZE], 0, PAGE_SIZE);
    }
}

#pragma GCC diagnostic pop
