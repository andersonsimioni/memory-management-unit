#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"

#include <iostream>

using namespace std;

extern char* algorithm;
extern int page_faults;//Page faults: XX
extern int disk_reads;//Disk reads: XX
extern int disk_writes;//Disk writes: XX

class Page_Replacement
{
public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    static void page_fault_handler(Page_Table *pt, int page);
};

#endif