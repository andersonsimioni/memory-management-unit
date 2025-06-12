#include "page_replacement.h"

/*
 * This this the method called when a page fault occurs. Your work begins here!
 */

int Page_Replacement::page_faults = 0;
int Page_Replacement::disk_reads = 0;
int Page_Replacement::disk_writes = 0;

vector<int> Page_Replacement::frame_to_page;
vector<int> Page_Replacement::page_to_frame;
vector<bool> Page_Replacement::frame_free;
queue<int> Page_Replacement::fifo_queue;

int Page_Replacement::nframes = 0;
int Page_Replacement::npages = 0;


void Page_Replacement::page_fault_handler(Page_Table *pt, int page )
{
    cout << "page fault on page #" << page << endl;


    static bool initialized = false;
    if (!initialized) {
        cout<<"inicializado!" << endl;
        nframes = pt->page_table_get_nframes();
        npages = pt->page_table_get_npages();

        frame_to_page.resize(nframes, -1); // -1 = frame vazio
        page_to_frame.resize(npages, -1);  // -1 = página não está na memória
        frame_free.resize(nframes, true);  // todos os frames estão livres inicialmente

        initialized = true;
    }

    // Apenas se page == frame
    if (npages == nframes) {
        cout<<"npages == nframes" << endl;
        pt->page_table_set_entry(page, page, PROT_READ | PROT_WRITE);
        return;
    }   

    //Caso padrão, vamos analizar qual o tratamento da página requerida
    int frame;
    int bits;
    pt->page_table_get_entry(page, &frame, &bits);

    cout << bits;
    cout << frame;
    
    if (bits & PROT_READ) {
        // Falta de permissão de escrita: adicionar PROT_WRITE
        pt->page_table_set_entry(page, frame, PROT_READ | PROT_WRITE);
        return;
    } else if (bits == 0) {
        // Página não está na memória. Precisa carregar do disco.
        // Verificar se há frame livre, senão, fazer substituição.
        page_faults++;
        // Verifica se existe frame livre
        int frame_index = -1;
        for (int i = 0; i < nframes; ++i) {
            if (frame_free[i]) {
                frame_index = i;
                break;
            }
        }

        if (frame_index == -1) {
            // Não há frame livre, usar algoritmo de substituição

            //IMPLEMENTE AQUI OS ALGORITMOS
        }
    }
    exit(1);
}

void Page_Replacement::print_stats() {
    cout << "Page faults: " << page_faults << endl;
    cout << "Disk reads: " << disk_reads << endl;
    cout << "Disk writes: " << disk_writes << endl;
}