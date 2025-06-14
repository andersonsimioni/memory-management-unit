#include "page_replacement.h"
#include <string.h> 

/*
 * This this the method called when a page fault occurs. Your work begins here!
 */

std::string Page_Replacement::algorithm = "";
vector<bool> Page_Replacement::frame_free;
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
        frame_free.resize(nframes, true);  // todos os frames estão livres inicialmente
        initialized = true;
        return;

    }

    //1 passo: checar se a página estava fora da page table
    int frame, bits;
    pt->page_table_get_entry(page, &frame, &bits);
    if (bits == 0) {  // bits == 0 → página não foi mapeada
        cout<<"página de fora da page table"<<endl;
    }

    //CASO ESPECIAL: Página já mapeada e em frame, passar de leitura para escrita
    if (bits & PROT_READ) {
        // Falta de permissão de escrita: adicionar PROT_WRITE
        pt->page_table_set_entry(page, frame, PROT_READ | PROT_WRITE);
        return;
    }

    //2 passo: Verifica se existe frame livre
    int frame_index = -1;
    for (int i = 0; i < nframes; ++i) {
        if (frame_free[i]) {
            frame_index = i;
                break;
        }
    }

    //3 passo: se frame index = pos, insere página na posição e atualiza page table senão, precisa substituir
    if (frame_index != -1){
        frame_free[frame_index] = false; //marca como ocupado
        //lê página do disco
        //insere no frame pos
        pt->page_table_set_entry(page, frame_index, PROT_READ); //atualiza página na page table
        cout<<"Entrou aqui!"<<endl;
        cout << "Página " << page << " mapeada no frame " << frame_index << endl;
        pt -> page_table_print_entry(page);
        return;
    } else {
        cout << "Sem frame livre, deve aplicar algoritmo de substituição" << endl;
        /*
        if (//se algoritmo == fifo){
            cout << "SELECIONADO: FIFO" << endl;
            //logica do fifo...
        }
        if (//se algoritmo == rand) == 0){
            cout << "SELECIONADO: RAND" << endl;
            //logica do rand...
        }
        if (//se algoritmo == custom) == 0){
            cout << "SELECIONADO: CUSTOM" << endl;
            //lógica do custom...
        }
        */
    }
}