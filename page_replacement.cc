#include "page_replacement.h"
#include <vector>
#include <queue>
#include <cstring>
#include <cassert>

static std::queue<int> page_fifo; //FIFO for current pages on RAM, used to replace the oldest
static std::vector<bool> frame_free_status; //Save if frame is free or not

// Handler de page fault
void Page_Replacement::page_fault_handler(Page_Table *pt, int page)
{
    cout << "page fault on page #" << page << endl;

    const int nframes = pt->page_table_get_nframes();
    const int npages  = pt->page_table_get_npages();
    if(frame_free_status.size() != nframes) frame_free_status.resize(nframes, true);

    //Search for free frame, if not found, get oldest in FIFO to replace
    int frame_to_use = -1;
    for (int i = 0; i < nframes; i++) if (frame_free_status[i]) { frame_to_use = i; frame_free_status[i] = false; break;}

    //Not found free frame, so replace the oldest in FIFO
    if (frame_to_use == -1 && !page_fifo.empty()) {
        int old_frame, old_bits;
        int oldest_page = page_fifo.front();
        page_fifo.pop();

        pt->page_table_get_entry(oldest_page, &old_frame, &old_bits);
        pt->page_table_set_entry(oldest_page, old_frame, 0); //Remove old frame mapping

        frame_to_use = old_frame; //set what frame will be used
    }

    pt->page_table_set_entry(page, frame_to_use, PROT_READ | PROT_WRITE);
    page_fifo.push(page); //Append most recent page on FIFO
    
    //IO Disk operations.. if necessary
}