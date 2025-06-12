#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"

#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <cassert>
#include <list>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <unordered_map>

using namespace std;

class Page_Replacement
{
private:
    static Page_Replacement* singleton_instance;
    Disk* disk = nullptr; //disk to save page data and protection bits

    char* algorithm;
    int page_faults;
    int disk_reads;
    int disk_writes;

    std::queue<int> page_queue; //used for FIFO
    std::vector<int> page_list; //used for RANDOM & CUSTOM
    std::vector<bool> frame_free_status; //set if frame are in usig

public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    static void page_fault_handler(Page_Table *pt, int page);

    static Page_Replacement* get_instance();

    void page_fault_handler_non_static(Page_Table *pt, int page);

    void print_statistics();

    void unload_page(Page_Table *pt, int page, int* old_frame, int* old_bits);

    void load_page(Page_Table *pt, int page, int frame_to_use);

    void page_replacement_delete();

    Page_Replacement(const char* algorithm, Disk* disk)
    {
        if(singleton_instance != nullptr) throw std::runtime_error("Page_Replacement already instanced!");
        
        int alg_name_len = strlen(algorithm);
        this->algorithm = new char[alg_name_len];
        memcpy(this->algorithm, algorithm, alg_name_len);

        this->disk = disk;
        singleton_instance = this;
    }
};

#endif