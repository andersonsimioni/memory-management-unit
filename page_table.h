#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H


/*
 * NÃO MODIFICAR ESSE ARQUIVO!
 */

#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <iostream>

using namespace std;

class Page_Table
{
public:
    static const unsigned int PAGE_SIZE = 4096;

public:
    typedef void (*page_fault_handler_t) (Page_Table *pt, int page);

public:
    /* Create a new page table, along with a corresponding virtual memory
     * that is "npages" big and a physical memory that is "nframes" bit
     * When a page fault occurs, the routine pointed to by "handler" will be called. 
     */
    Page_Table( int npages, int nframes, page_fault_handler_t handler);

    /* Delete a page table and the corresponding virtual and physical memories. */
    void page_table_delete();

    /*
     * Set the frame number and access bits associated with a page.
     * The bits may be any of PROT_READ, PROT_WRITE, or PROT_EXEC logical-ored together.
     */
    void page_table_set_entry(int page, int frame, int bits);

    /*
     * Get the frame number and access bits associated with a page.
     * "frame" and "bits" must be pointers to integers which will be filled with the current values.
     * The bits may be any of PROT_READ, PROT_WRITE, or PROT_EXEC logical-ored together.
     */

    void page_table_get_entry(int page, int *frame, int *bits);

    /* Return a pointer to the start of the virtual memory associated with a page table. */
    void * page_table_get_virtmem();

    /* Return a pointer to the start of the physical memory associated with a page table. */
    void * page_table_get_physmem();

    /* Return the total number of frames in the physical memory. */
    int page_table_get_nframes();

    /* Return the total number of pages in the virtual memory. */
    int page_table_get_npages();

    /* Print out the page table entry for a single page. */
    void page_table_print_entry(int page);

    /* Print out the state of every page in a page table. */
    void page_table_print();

public:
    static Page_Table *the_page_table;

private:
    static void internal_fault_handler( int signum, siginfo_t *info, void *context )
    {
        #ifdef i386
            char *addr = (char*)(((struct ucontext *)context)->uc_mcontext.cr2);
        #else
            char *addr = (char*) info->si_addr;
        #endif

        Page_Table *pt = Page_Table::the_page_table;

        if(pt) {
            int page = (addr - pt->virtmem) / PAGE_SIZE;

            if(page >= 0 && page < pt->npages) {
                pt->handler(pt, page);
                return;
            }
        }

        cout << "PAGE TABLE: segmentation fault at address " << addr << endl;
        abort();
    }

private:
    int fd;
    char *virtmem;
    int npages;
    char *physmem;
    int nframes;
    int *page_mapping;
    int *page_bits;
    struct sigaction sa;
    page_fault_handler_t handler;
};

#endif
