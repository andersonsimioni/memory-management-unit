#include "page_replacement.h"
#include <vector>
#include <queue>
#include <cstring>
#include <cassert>
#include <list>
#include <cstdlib>
#include <unordered_map>
#include <ctime>
#include "disk.h"

// this is a pointer to the disk we use to save metadata like protection bits
static bool use_disk = true;
static Disk* disk = nullptr;

// this queue is for FIFO algorithm, it keeps track of the order pages entered memory
static std::queue<int> page_queue;

// this vector holds all pages currently in memory for RANDOM and CUSTOM algorithms
static std::vector<int> page_list;

// this vector keeps info about which frames are still free and which are used
static std::vector<bool> frame_free_status;

// we define the types of page replacement algorithms we support
enum algorithm_enum
{
    FIFO,   // first page in will be the first out (simple)
    RANDOM, // remove a random page, no logic just chance
    CUSTOM  // a custom logic based on protection bits of the page
};

// we choose which algorithm to use here
const algorithm_enum algorithm = FIFO;

//save phys memory page into disk to replace the page with backup
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

    char buffer[disk->DISK_BLOCK_SIZE];
    char* physmem = (char*)pt->page_table_get_physmem();
    memcpy(buffer, physmem, disk->DISK_BLOCK_SIZE);
    buffer[disk->DISK_BLOCK_SIZE-1] = (char)*old_bits;

    disk->write(page, buffer);

    //unlink page from frame
    pt->page_table_set_entry(page, *old_frame, 0);
}


//load phys memory page from disk
void load_page_from_disk(Page_Table *pt, int page, int frame_to_use) {
    if (disk == nullptr) 
    {
        cout<<"mapping page "<<page<<" to frame "<<frame_to_use<<endl; 
        pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE); 
        return;
    }
    
    int page_size = pt->PAGE_SIZE + 1; //+1 for protection bits
    char buffer[page_size];
    disk->read(page, buffer); //1:1 -> 1 page = 1 block to simplify..

    int protection_bits = buffer[pt->PAGE_SIZE - 1]; // last byte of block
    if(protection_bits == 0) protection_bits = PROT_READ | PROT_WRITE;
    char data[pt->PAGE_SIZE];
    
    char* physmem = (char*)pt->page_table_get_physmem();
    memcpy(physmem, buffer, pt->PAGE_SIZE);

    // map page again
    cout<<"(FROM DISK) mapping page "<<page<<" to frame "<<frame_to_use<<endl;
    pt->page_table_set_entry(page, frame_to_use, protection_bits);
}


// this is the page fault handler, it runs when the system tries to use a page that is not in memory
void Page_Replacement::page_fault_handler(Page_Table *pt, int page)
{    
    
    if(disk == nullptr && use_disk)//start disk with not started yet and use disk
    {
        cout << "initializing disk..." << endl;

        Disk sample("", 0); //just to get block size...

        int total_pages = pt->page_table_get_npages(); // how many pages are in the system
        int blocks = (total_pages * (pt->PAGE_SIZE + 1)) / sample.DISK_BLOCK_SIZE; // blocks for page data, +1 page size (PROT_..)
        disk = new Disk("myvirtualdisk", blocks); // create the actual disk

        cout << "disk started successfully!" << endl;
    }

    cout << "page fault on page #" << page << endl;

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
    if(algorithm == CUSTOM)
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
    else if(algorithm == RANDOM)
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
    else if(algorithm == FIFO)
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
