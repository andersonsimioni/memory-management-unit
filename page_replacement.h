#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"

#include <iostream>
#include <vector>
#include <queue>
#include <cstring>

using namespace std;

/**
 * @class Page_Replacement
 * @brief Tratador de faltas de página
 * Classe contendo os métodos usados para tratamento de page faults
 * por meio dos algoritmos de substituição de páginas (FIFO, RANDOM E CUSTOM).
 */
class Page_Replacement
{
private:
    /**
     * @brief Instância singleton da classe.
     * Será utilizada para passagem dos argumentos de entrada
     */
    static Page_Replacement* singleton_instance;

    /**
     * @brief Disco usado para armazenar páginas.
     */
    Disk* disk = nullptr;

    /**
     * @brief Nome do algoritmo de substituição a ser usado.
     */
    char* algorithm;

    /**
     * @brief Contador de faltas de página.
     */
    int page_faults;

    /**
     * @brief Contador de leituras do disco.
     */
    int disk_reads;

    /**
     * @brief Contador de escritas no disco.
     */
    int disk_writes;

    /**
     * @brief Fila das páginas presentes em memória, usada em FIFO.
     */
    std::queue<int> page_queue;

    /**
     * @brief Mapeia quais páginas estão em quais frames (usado em algoritmo CUSTOM e RAND).
     */
    std::vector<int> frame_to_page_map;


public:
    /**
     * @brief Método estático chamado quando ocorre uma falta de páginas
     * @param pt Ponteiro para a tabela de páginas.
     * @param page Número da página requisitada.
     */
    static void page_fault_handler(Page_Table *pt, int page);

    /**
     * @brief Versão não estática do método handler para ser usada com instancia
     * Método principal do handler, onde está presente os algoritmos de substituição 
     * e as funções de leitura/escrita em disco.
     * @param pt Ponteiro para a tabela de páginas.
     * @param page Número da página requisitada.
     */
    void page_fault_handler_non_static(Page_Table *pt, int page);

    /**
     * @brief Obtém a instância singleton da classe.
     *
     * @return Ponteiro para a instância.
     */
    static Page_Replacement* get_instance();

    /**
     * @brief Realiza os prints das estatísticas após execução.
     *Mostra o número de page faults, leituras e escritas em disco.
     */
    void print_statistics();

    /**
     * @brief Realiza a substituição de páginas na memória.
     * Salva, se necessário, a página antiga no disco, e carrega a nova.
     * @param pt Ponteiro para a tabela de páginas.
     * @param old_page Página a ser substituída.
     * @param new_page Nova página.
     * @param frame_to_use Informa que não há espaço em nenhum frame.
     */
    void swap_page_frames(Page_Table *pt, int old_page, int new_page, int frame_to_use = -1);

    /**
     * @brief Desaloca recursos alocados pela instância.
     */
    void page_replacement_delete();

    /**
     * @brief Construtor da classe Page_Replacement.
     * Inicializa a instância singleton com o algoritmo especificado.
     * @param algorithm Nome do algoritmo de substituição de páginas selecionado.
     * @param disk Disco que será usado para ler/escrever páginas.
     *
     * @throws std::runtime_error Se já houver uma instância
     */
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