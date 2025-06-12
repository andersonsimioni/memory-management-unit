#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"

#include <iostream>

using namespace std;
class Page_Replacement
{
private:
    char* algorithm;
    int page_faults;
    int disk_reads;
    int disk_writes;

    Disk* disk = nullptr; //disk to save page data and protection bits

    std::queue<int> page_queue; //used for FIFO
    std::vector<int> page_list; //used for RANDOM & CUSTOM
    std::vector<bool> frame_free_status; //set if frame are in using

public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    void page_fault_handler(Page_Table *pt, int page);

    void print_statistics();

    void unload_page(Page_Table *pt, int page, int* old_frame, int* old_bits);

    void load_page(Page_Table *pt, int page, int frame_to_use);

    Page_Replacement(char* algorithm, Disk* disk)
    {
        this->algorithm = algorithm;
        this->disk = disk;
    }
};

#endif