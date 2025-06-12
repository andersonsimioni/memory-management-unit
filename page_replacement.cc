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

void Page_Replacement::swap_page_frames(Page_Table *pt, int old_page, int new_page, int frame_to_use)
{
    cout<<"swapping page "<<old_page<<" for page "<<new_page<<endl;
    int old_page_frame = -1, old_page_bits = -1;
    pt->page_table_get_entry(old_page, &old_page_frame, &old_page_bits);
    if(old_page_bits & PROT_WRITE)
    {
        char* physmem = (char*)pt->page_table_get_physmem();
        disk->write(old_page, &physmem[pt->PAGE_SIZE*old_page_frame]);
        disk_writes++;
    }

    char* physmem = (char*)pt->page_table_get_physmem();
    disk->read(new_page, &physmem[pt->PAGE_SIZE*old_page_frame]); //1:1 -> 1 page = 1 block to simplify..
    disk_reads++;

    frame_to_page_map[old_page_frame] = new_page;
    pt->page_table_set_entry(new_page, old_page_frame, PROT_READ | PROT_WRITE);
    pt->page_table_set_entry(old_page, 0, 0);
}

// this is the page fault handler, it runs when the system tries to use a page that is not in memory
void Page_Replacement::page_fault_handler_non_static(Page_Table *pt, int page)
{    
    pt->page_table_print();
    //abort();

    cout << "page fault on page #" << page << endl;
    page_faults++;

    int old_page;
    const int nframes = pt->page_table_get_nframes();
    const int npages  = pt->page_table_get_npages();

    if(frame_to_page_map.size() != nframes) frame_to_page_map.resize(nframes, -1);
    if(page_list.size() != npages) page_list.resize(npages, -1);

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
    
    if(!strcmp(algorithm, (char*)"custom"))
    {
        if(frame_to_use == -1)
        {
            // in custom algorithm, we select the frame follow by (frame = page MOD nframes
            cout << "using algorithm CUSTOM to solve"<<endl;
            int frame = page % nframes;
            old_page = frame_to_page_map[frame];
            frame_to_page_map[frame] = page;    
        }
    }
    else if(!strcmp(algorithm, (char*)"rand"))
    {
        if(frame_to_use == -1)
        {
            cout << "using algorithm RANDOM to solve"<<endl;

            // we just pick a random page and remove it
            int random_index = rand() % page_list.size();
            old_page = page_list[random_index];
            page_list[random_index] = page; // replace it with the new page
        }
    }
    else if(!strcmp(algorithm, (char*)"fifo"))
    {
        if(frame_to_use == -1)
        {
            cout << "using algorithm FIFO to solve"<<endl;
            old_page = page_queue.front();
            page_queue.pop();
        }
        
        page_queue.push(page); // add our new page to the end of the queue
    }
    else
    {
        cout<<"invalid algorithm"<<endl;
        abort();
    }

    if(frame_to_use == -1)
    {
        swap_page_frames(pt, old_page, page);
    }
    else
    {
        pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE);
    }
    
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