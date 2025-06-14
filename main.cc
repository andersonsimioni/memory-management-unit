#include "disk.h"
#include "program.h"
#include "page_table.h"
#include "page_replacement.h"
#include <cstring>


/**
 * MODO DE USO:
 * virtmem <npages> <nframes> <rand|fifo|custom> <alpha|beta|gamma|delta>
 * 
 * onde:
 * - npages: número total de páginas
 * - nframes: número de frames disponíveis
 * - rand|fifo|custom: algoritmo escolhido para substituição de páginas
 * - alpha|beta|gamma|delta: programas que realizarão operações com a memória
 */


/**
 * @brief Função main que inicializa as demais classes da simulação.
 * 
 * @param argc Número de argumentos de entrada.
 * @param argv Vetor de strings com os argumentos:
 *  - argv[1]: número de páginas
 *  - argv[2]: número de frames
 *  - argv[3]: algoritmo de substituição ("rand", "fifo" ou "custom")
 *  - argv[4]: programa de testes ("alpha", "beta", "gamma" ou "delta")
 * @return int Código de retorno (0).
 */
int main(int argc, char *argv[])
{
	// Verifica se o número correto de argumentos foi passado
	if(argc != 5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <alpha|beta|gamma|delta>\n");
		return 1;
	}
	
	// Lê e valida os argumentos
	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char *algorithm = argv[3];
	const char *program = argv[4];
	if(npages <=0 || nframes <= 0)
	{
		cout<<"invalid npages or nframes"<<endl;
		abort();
	}

	// Instancia o disco
	Disk disk("myvirtualdisk", npages);

	// Instancia o programa de testes
    Program my_program;
	
	// Instancia o handler de faltas de páginas
	Page_Replacement page_fault_manager(algorithm, &disk);

	// Instancia a tabela de páginas com o número de páginas, frames e handler
    Page_Table pt(npages, nframes, Page_Replacement::page_fault_handler);
	
	// Obtém o ponteiro para a memória virtual simulada
	unsigned char *virtmem = (unsigned char *) pt.page_table_get_virtmem();
	
	// Executa o programa de teste com o padrão de acesso escolhido
    if(!strcmp(program,"alpha")) {
		my_program.alpha(virtmem, npages * Page_Table::PAGE_SIZE);

	} else if(!strcmp(program,"beta")) {
		my_program.beta(virtmem, npages * Page_Table::PAGE_SIZE);

	} else if(!strcmp(program,"gamma")) {
		my_program.gamma(virtmem, npages * Page_Table::PAGE_SIZE);

	} else if(!strcmp(program,"delta")) {
		my_program.delta(virtmem, npages * Page_Table::PAGE_SIZE);

	} else {
		cout << "unknown program: " << argv[4] << endl;
		return 1;
	}

	// Libera os recursos da tabela de páginas e fecha o disco
    pt.page_table_delete();
	disk.close_disk();

	// Imprime estatísticas de execução
	page_fault_manager.print_statistics();

	// Libera recursos da instância singleton
	page_fault_manager.get_instance()->page_replacement_delete();

	return 0;
}