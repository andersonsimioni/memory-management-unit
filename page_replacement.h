#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"
#include "disk.h"

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Page_Replacement
{
private:
static int page_faults;
static int disk_reads;
static int disk_writes;
static int nframes;
static int npages;

static std::vector<int> frame_to_page; // frame -> page
static std::vector<int> page_to_frame; // page -> frame
static std::vector<bool> frame_free;   // indica quais frames estão livres
static std::queue<int> fifo_queue;     // usado para FIFO

public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    static void page_fault_handler(Page_Table *pt, int page);
    static void print_stats();
};

#endif