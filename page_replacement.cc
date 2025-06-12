#include "disk.h"
#include "page_replacement.h"

#include <vector>
#include <queue>
#include <cstring>
#include <cassert>
#include <list>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <unordered_map>

Page_Replacement* Page_Replacement::singleton_instance = nullptr;

void Page_Replacement::print_statistics()
{
    cout<<"Page faults: "<<page_faults<<endl;
	cout<<"Disk reads:"<<disk_reads<<endl;
	cout<<"Disk writes: "<<disk_writes<<endl;
}

//save data and protection bits into disk, if use_disk is true..
void Page_Replacement::unload_page(Page_Table *pt, int page, int* old_frame, int* old_bits) {
    pt->page_table_get_entry(page, old_frame, old_bits); 
    cout<<"clearing & saving page "<<page<<" on disk , frame: "<<*old_frame<<endl;
    if(*old_frame > pt->page_table_get_nframes())
    {
        pt->page_table_print();
        cout<<("page table returned wrong value for frame")<<endl;
        abort();
    }

    char* physmem = (char*)pt->page_table_get_physmem();
    //disk->write(page, physmem);
    disk_writes++;

    //unlink page from frame
    //pt->page_table_set_entry(page, 0, PROT_NONE);
    frame_to_page_map[*old_frame] = -1;
}

//load data and protection bits from disk
void Page_Replacement::load_page(Page_Table *pt, int page, int frame_to_use) {
    char* physmem = (char*)pt->page_table_get_physmem();
    //disk->read(page, physmem); //1:1 -> 1 page = 1 block to simplify..
    disk_reads++;

    // map page again
    cout<<"(FROM DISK) mapping page "<<page<<" to frame "<<frame_to_use<<endl;
    pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE);
    frame_to_page_map[frame_to_use] = page;

    //double check..
    int chk_frame = -1, chk_bits = -1;
    pt->page_table_get_entry(page, &chk_frame, &chk_bits);
    if(chk_frame != frame_to_use)
    {
        cout<<"page table not respecting logic, frame_to_use: "<<frame_to_use<<", chk_frame: "<<chk_frame<<endl;
        abort();
    }
}

// this is the page fault handler, it runs when the system tries to use a page that is not in memory
void Page_Replacement::page_fault_handler_non_static(Page_Table *pt, int page)
{    
    //pt->page_table_print();
    //abort();

    cout << "page fault on page #" << page << endl;
    page_faults++;

    const int nframes = pt->page_table_get_nframes();
    const int npages  = pt->page_table_get_npages();
    if(frame_to_page_map.size() != nframes) frame_to_page_map.resize(nframes, -1);

    // search for a free frame
    int frame_to_use = -1;
    for (int i = 0; i < nframes; i++) 
    {
        if (frame_to_page_map[i] == -1) 
        {
            frame_to_use = i;             // found a free frame
            frame_to_page_map[i] = page;  // set page that use it
            break;
        }
    }
    
    int old_frame = -1, old_bits = -1;
    if(!strcmp(algorithm, (char*)"custom"))
    {
        // in custom algorithm, we select the frame follow by (frame = page MOD nframes)

        if (frame_to_use == -1)
        {
            cout << "using algorithm CUSTOM to solve"<<endl;

            int frame = page % nframes;
            unload_page(pt, frame_to_page_map[frame], &old_frame, &old_bits); // backup its info
            frame_to_use = old_frame;
            //frame_to_page_map[frame] = page;
        }
    }
    else if(!strcmp(algorithm, (char*)"rand"))
    {
        if (frame_to_use == -1)
        {
            cout << "using algorithm RANDOM to solve"<<endl;

            // we just pick a random page and remove it
            int random_index = rand() % page_list.size();
            int old_page = page_list[random_index];

            unload_page(pt, old_page, &old_frame, &old_bits); // save the old one's info

            frame_to_use = old_frame;
            page_list[random_index] = page; // replace it with the new page
        }
        else
        {
            page_list.push_back(page); // if frame was free, just add the page normally
        }
    }
    else if(!strcmp(algorithm, (char*)"fifo"))
    {
        if(frame_to_use == -1 && !page_queue.empty())
        {
            cout << "using algorithm FIFO to solve"<<endl;

            // remove the page that entered memory first
            int oldest_page = page_queue.front();
            page_queue.pop();

            unload_page(pt, oldest_page, &old_frame, &old_bits);
            frame_to_use = old_frame;
        }

        page_queue.push(page); // add our new page to the end of the queue
    }
    else
    {
        cout<<"invalid algorithm"<<endl;
        abort();
    }

    load_page(pt, page, frame_to_use);
    cout<<"page fault fixed"<<endl;
}

Page_Replacement* Page_Replacement::get_instance()
{
    return Page_Replacement::singleton_instance;
}

void Page_Replacement::page_fault_handler(Page_Table *pt, int page)
{
    Page_Replacement* instance = Page_Replacement::get_instance();
    instance->page_fault_handler_non_static(pt, page);
}

void Page_Replacement::page_replacement_delete()
{    
    std::cout<<"clearing memory.."<<endl;

    if (this->algorithm != nullptr) 
    {
        delete[] this->algorithm;
        this->algorithm = nullptr;
    }

    while (!page_queue.empty()) page_queue.pop();
    page_list.clear();
    frame_to_page_map.clear();

    page_faults = 0;
    disk_reads = 0;
    disk_writes = 0;

    singleton_instance = nullptr;

    cout<<"memory clean with success"<<endl;
}