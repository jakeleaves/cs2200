#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * Checkout PDF sections 4 for this problem
 * 
 * In this problem, you will initialize the frame_table pointer. The frame table will
 * be located at physical address 0 in our simulated memory. You should zero out the 
 * entries in the frame table, in case for any reason physical memory is not clean.
 * 
 * HINTS:
 *      - mem: Simulated physical memory already allocated for you.
 *      - PAGE_SIZE: The size of one page
 * ----------------------------------------------------------------------------------
 */
void system_init(void) {
    // TODO: initialize the frame_table pointer.
    frame_table = (fte_t*)mem;
    memset(frame_table, 0, PAGE_SIZE);
    frame_table -> protected = 1;
}

/**
 * --------------------------------- PROBLEM 5 --------------------------------------
 * Checkout PDF section 6 for this problem
 * 
 * Takes an input virtual address and performs a memory operation.
 * 
 * @param addr virtual address to be translated
 * @param rw   'r' if the access is a read, 'w' if a write
 * @param data If the access is a write, one byte of data to write to our memory.
 *             Otherwise NULL for read accesses.
 * 
 * HINTS:
 *      - Remember that not all the entry in the process's page table are mapped in. 
 *      Check what in the pte_t struct signals that the entry is mapped in memory.
 * ----------------------------------------------------------------------------------
 */
uint8_t mem_access(vaddr_t addr, char rw, uint8_t data) {
    // TODO: translate virtual address to physical, then perfrom the specified operation

    /* Either read or write the data to the physical address
       depending on 'rw' */

    vpn_t vpn = vaddr_vpn(addr);
    pte_t* page_table = (pte_t*) (mem + PTBR * PAGE_SIZE);
    pte_t* page_table_entry = &page_table[vpn];
    uint16_t offset = vaddr_offset(addr);

    /* Aallocate a page for the page table if invalid. */
    if (!page_table_entry -> valid) {
        page_fault(addr);
        stats.page_faults++;
    }
    /* Update the referenced bit of frame table entry. */
    fte_t* frame_table_entry = (fte_t*) (frame_table + page_table_entry -> pfn);
    frame_table_entry -> referenced = 1;

    if (rw == 'r') {
        stats.page_faults++;

    } else {
        *(mem + page_table_entry -> pfn * PAGE_SIZE + offset) = data;
        page_table_entry -> dirty = 1;
        stats.writebacks++;    
    }
    stats.accesses++;

    return *(mem + page_table_entry -> pfn * PAGE_SIZE + offset);
}
