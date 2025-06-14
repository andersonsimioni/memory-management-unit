#include "disk.h"
#include "page_replacement.h"

//singleton inicia como nullptr
Page_Replacement* Page_Replacement::singleton_instance = nullptr;

/**
 * @brief Imprime os resultados da execução.
 */
void Page_Replacement::print_statistics()
{
    cout<<"Page faults: "<<page_faults<<endl;
	cout<<"Disk reads:"<<disk_reads<<endl;
	cout<<"Disk writes: "<<disk_writes<<endl;
}

/**
 * @brief Realiza a substituição de páginas entre memória e disco.
 * 
 * Se a página antiga tiver sido modificada, ela é escrita no disco antes da substituição.
 * Feito isso, a nova página é lida do disco e colocada na posição correspondente da memória física.
 * 
 * @param pt Ponteiro para a tabela de páginas.
 * @param old_page Página a ser substituída.
 * @param new_page Nova página.
 * @param frame_to_use Informa que não há espaço em nenhum frame.
 */
void Page_Replacement::swap_page_frames(Page_Table *pt, int old_page, int new_page, int frame_to_use)
{
    int old_page_frame = -1, old_page_bits = -1;
    pt->page_table_get_entry(old_page, &old_page_frame, &old_page_bits);

    // Se a página foi escrita, salvá-la no disco
    if(old_page_bits & PROT_WRITE)
    {
        char* physmem = (char*)pt->page_table_get_physmem();
        disk->write(old_page, &physmem[pt->PAGE_SIZE*old_page_frame]);
        disk_writes++;
    }

    // Lê a nova página do disco
    char* physmem = (char*)pt->page_table_get_physmem();
    //1:1 -> 1 página = 1 bloco
    disk->read(new_page, &physmem[pt->PAGE_SIZE*old_page_frame]);
    disk_reads++;

    // Atualiza o mapeamento de frames
    frame_to_page_map[old_page_frame] = new_page;
    pt->page_table_set_entry(new_page, old_page_frame, PROT_READ);
    pt->page_table_set_entry(old_page, 0, 0);
}

/**
 * @brief Método principal para o tratamento de page fault
 * 
 * Utiliza as informações contidas nos atributos do singleton
 * Implementa os algoritmos de substituição: FIFO, RAND e CUSTOM.
 * Tenta alocar a página um frame livre, caso todos os frames cheios, substitui uma página.
 * 
 * @param pt Ponteiro para a tabela de páginas.
 * @param page Número da página requisitada.
 */
void Page_Replacement::page_fault_handler_non_static(Page_Table *pt, int page)
{    
    page_faults++;
    
    int old_page;
    const int nframes = pt->page_table_get_nframes();
    frame_to_page_map.resize(nframes, -1);

    // Se a página já está mapeada, apenas adiciona permissão de WRITE
    for (int i = 0; i < static_cast<int>(frame_to_page_map.size()); i++)
    {
        if(frame_to_page_map[i] == page)
        {
            int aux_frame, aux_bits;
            pt->page_table_get_entry(page, &aux_frame, &aux_bits);
            pt->page_table_set_entry(page, aux_frame, PROT_READ | PROT_WRITE);
            return;
        }
    }
    

    // Procura um frame livre
    int frame_to_use = -1;
    for (int i = 0; i < nframes; i++) 
    {
        if (frame_to_page_map[i] == -1) 
        {
            // Achou um frame livre
            frame_to_use = i;
            // Mapeia a página no frame livre
            frame_to_page_map[i] = page;
            break;
        }
    }
    
    //ALGORITMO CUSTOM
    if(!strcmp(algorithm, (char*)"custom"))
    {
        if(frame_to_use == -1)
        {
            //O frame da página selecionada para ser substituída é: frame = page MOD nframes
            int frame = page % nframes;
            old_page = frame_to_page_map[frame];
            frame_to_page_map[frame] = page;    
        }
    }

    //ALGORITMO RAND
    else if(!strcmp(algorithm, (char*)"rand"))
    {
        if(frame_to_use == -1)
        {
            // Escolhe uma página aleatória para ser substituída
            int random_index = rand() % frame_to_page_map.size();
            old_page = frame_to_page_map[random_index];
            frame_to_page_map[random_index] = page;
        }
    }
    //ALGORITMO FIFO
    else if(!strcmp(algorithm, (char*)"fifo"))
    {
        if(frame_to_use == -1)
        {
            // A primeira página da fila será a selecionada para ser substituída
            old_page = page_queue.front();
            page_queue.pop();
        }
        
        // Atualiza fila
        page_queue.push(page);
    }

    else
    {
        //cout<<"invalid algorithm"<<endl;
        abort();
    }

    // Realiza a substituição
    if(frame_to_use == -1) swap_page_frames(pt, old_page, page);
    // Ou insere no frame vazio e atualiza a page table
    else pt->page_table_set_entry(page, frame_to_use, PROT_READ);
}

/**
 * @brief Método estático do Handler.
 *
 * Encaminha para o método não estático da instância singleton e
 * permite que seja passado os argumentos de entrada de forma simples.
 * 
 * @param pt Ponteiro para a tabela de páginas.
 * @param page Página requisitada.
 */
void Page_Replacement::page_fault_handler(Page_Table *pt, int page)
{
    Page_Replacement* instance = Page_Replacement::get_instance();
    instance->page_fault_handler_non_static(pt, page);
}

/**
 * @brief Getter da instância singleton de Page_Replacement.
 * 
 * @return Ponteiro para a instância atual.
 */
Page_Replacement* Page_Replacement::get_instance()
{
    return Page_Replacement::singleton_instance;
}

/**
 * @brief Libera recursos alocados pela instância singleton.
 * 
 * Apaga atributo algoritmo, limpa fila de páginas da FIFO e o vetor de mapeamento de frames,
 * reseta contadores e remove a instância singleton.
 */
void Page_Replacement::page_replacement_delete()
{
    if (this->algorithm != nullptr) 
    {
        delete[] this->algorithm;
        this->algorithm = nullptr;
    }

    while (!page_queue.empty()) page_queue.pop();
    frame_to_page_map.clear();

    page_faults = 0;
    disk_reads = 0;
    disk_writes = 0;

    singleton_instance = nullptr;
}