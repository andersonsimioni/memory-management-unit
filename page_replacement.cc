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
    if (disk == nullptr) 
    {
        cout<<"clearing page "<<page<<endl; 
        pt->page_table_get_entry(page, old_frame, old_bits); 
        pt->page_table_set_entry(page, *old_frame, 0); 
        return; 
    }

    pt->page_table_get_entry(page, old_frame, old_bits); 
    cout<<"clearing & saving page "<<page<<" on disk , frame: "<<*old_frame<<endl;

    char* physmem = (char*)pt->page_table_get_physmem();
    disk->write(page, physmem);
    disk_writes++;

    //unlink page from frame
    pt->page_table_set_entry(page, *old_frame, 0);
    frame_free_status[*old_frame] = true;
}

//load data and protection bits from disk
void Page_Replacement::load_page(Page_Table *pt, int page, int frame_to_use) {
    if (disk == nullptr) 
    {
        cout<<"mapping page "<<page<<" to frame "<<frame_to_use<<endl; 
        pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE); 
        return;
    }
    
    char* physmem = (char*)pt->page_table_get_physmem();
    disk->read(page, physmem); //1:1 -> 1 page = 1 block to simplify..
    disk_reads++;

    // map page again
    cout<<"(FROM DISK) mapping page "<<page<<" to frame "<<frame_to_use<<endl;
    pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE);
    frame_free_status[frame_to_use] = false;
}

// this is the page fault handler, it runs when the system tries to use a page that is not in memory
void Page_Replacement::page_fault_handler_non_static(Page_Table *pt, int page)
{    
    cout << "page fault on page #" << page << endl;
    page_faults++;

    const int nframes = pt->page_table_get_nframes(); // how many frames we got in RAM
    const int npages  = pt->page_table_get_npages();  // total pages the system can handle

    // make sure our free status vector is the right size
    if(frame_free_status.size() != nframes) frame_free_status.resize(nframes, true);

    // search for a free frame
    int frame_to_use = -1;
    for (int i = 0; i < nframes; i++) {
        if (frame_free_status[i]) {
            frame_to_use = i;              // found a free frame
            frame_free_status[i] = false;  // mark it as used
            break;
        }
    }
    
    int old_frame = -1, old_bits = -1;
    if(!strcmp(algorithm, (char*)"custom"))
    {
        // in custom algorithm, we try to remove the page with lowest "importance"
        // we decide importance by looking at the protection flags (read/write)

        if (frame_to_use == -1)
        {
            cout << "using algorithm CUSTOM to solve"<<endl;

            int lowest_priority_page = -1;
            int lowest_priority = 999;

            // we go through every page in memory and calculate its priority
            for (int p : page_list)
            {
                int frame, bits;
                pt->page_table_get_entry(p, &frame, &bits);

                int priority = 0;
                if(bits == (PROT_READ | PROT_WRITE)) priority = 3; // very active page
                else if(bits == (PROT_WRITE)) priority = 2;        // write only
                else if(bits == (PROT_READ)) priority = 1;         // read only
                // if no bits are set, it's probably unused, so priority stays 0

                if (priority < lowest_priority){ // we try to find the lowest one
                    lowest_priority = priority;
                    lowest_priority_page = p;
                }
            }

            unload_page(pt, lowest_priority_page, &old_frame, &old_bits); // backup its info
            frame_to_use = old_frame;

            // remove it from our list of current pages
            page_list.erase(page_list.begin() + lowest_priority_page);
        }

        page_list.push_back(page); // add the new page we just loaded
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

    load_page(pt, page, frame_to_use);
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