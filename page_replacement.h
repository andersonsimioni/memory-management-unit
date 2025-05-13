#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"

#include <iostream>

using namespace std;

class Page_Replacement
{
public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    static void page_fault_handler(Page_Table *pt, int page);
};

#endif