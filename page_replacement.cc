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

extern char* algorithm = "fifo";

static bool use_disk = true; //true to use or false to not use the disk
static Disk* disk = nullptr; //disk to save page data and protection bits

static std::queue<int> page_queue; //used for FIFO
static std::vector<int> page_list; //used for RANDOM & CUSTOM
static std::vector<bool> frame_free_status; //set if frame are in using
static std::vector<int> protection_bits; //backup for protection bits

//save data and protection bits into disk, if use_disk is true..
void save_page_on_disk(Page_Table *pt, int page, int* old_frame, int* old_bits) {
    if (disk == nullptr) 
    {
        cout<<"clearing page "<<page<<endl; 
        pt->page_table_get_entry(page, old_frame, old_bits); 
        pt->page_table_set_entry(page, *old_frame, 0); 
        return; 
    }

    cout<<"clearing & saving page "<<page<<" on disk "<<endl;
    pt->page_table_get_entry(page, old_frame, old_bits); 

    char* physmem = (char*)pt->page_table_get_physmem();
    disk->write(page, physmem);

    //unlink page from frame
    pt->page_table_set_entry(page, *old_frame, 0);
}


//load data and protection bits from disk
void load_page_from_disk(Page_Table *pt, int page, int frame_to_use) {
    if (disk == nullptr) 
    {
        cout<<"mapping page "<<page<<" to frame "<<frame_to_use<<endl; 
        pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE); 
        return;
    }
    
    char* physmem = (char*)pt->page_table_get_physmem();
    disk->read(page, physmem); //1:1 -> 1 page = 1 block to simplify..

    // map page again
    cout<<"(FROM DISK) mapping page "<<page<<" to frame "<<frame_to_use<<endl;
    pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE);
}


// this is the page fault handler, it runs when the system tries to use a page that is not in memory
void Page_Replacement::page_fault_handler(Page_Table *pt, int page)
{    
    if(disk == nullptr && use_disk)//start disk with not started yet and use disk
    {
        cout << "initializing disk..." << endl;
        disk = new Disk("myvirtualdisk", pt->page_table_get_npages()); // open the current disk
        cout << "disk started successfully!" << endl;
    }

    cout << "page fault on page #" << page << endl;
    cout << "using algorithm " << algorithm << " to solve"<<endl;

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
    if(strcmp(algorithm, "custom"))
    {
        // in custom algorithm, we try to remove the page with lowest "importance"
        // we decide importance by looking at the protection flags (read/write)

        if (frame_to_use == -1)
        {
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

            save_page_on_disk(pt, lowest_priority_page, &old_frame, &old_bits); // backup its info
            frame_to_use = old_frame;

            // remove it from our list of current pages
            page_list.erase(page_list.begin() + lowest_priority_page);
        }

        page_list.push_back(page); // add the new page we just loaded
    }
    else if(strcmp(algorithm, "rand"))
    {
        if (frame_to_use == -1)
        {
            // we just pick a random page and remove it
            int random_index = rand() % page_list.size();
            int old_page = page_list[random_index];

            save_page_on_disk(pt, old_page, &old_frame, &old_bits); // save the old one's info

            frame_to_use = old_frame;
            page_list[random_index] = page; // replace it with the new page
        }
        else
        {
            page_list.push_back(page); // if frame was free, just add the page normally
        }
    }
    else if(strcmp(algorithm, "fifo"))
    {
        if(frame_to_use == -1 && !page_queue.empty())
        {
            // remove the page that entered memory first
            int oldest_page = page_queue.front();
            page_queue.pop();

            save_page_on_disk(pt, oldest_page, &old_frame, &old_bits);
            frame_to_use = old_frame;
        }

        page_queue.push(page); // add our new page to the end of the queue
    }

    load_page_from_disk(pt, page, frame_to_use);
}
