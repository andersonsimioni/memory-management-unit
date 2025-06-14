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
    static vector<bool> frame_free;
    static int nframes;
    static int npages;
public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    static void page_fault_handler(Page_Table *pt, int page);
    static void print_stats();
    static std::string algorithm; // "fifo", "rand", ou "custom"
};

#endif