#include "disk.h"
#include "program.h"
#include "page_table.h"
#include "page_replacement.h"
#include <cstring>

int main(int argc, char *argv[])
{
	if(argc != 5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <alpha|beta|gamma|delta>\n");
		return 1;
	}
	
	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char *algorithm = argv[3];
	const char *program = argv[4];
	cout<<strlen(algorithm)<<endl;
	if(npages <=0 || nframes <= 0)
	{
		cout<<"invalid npages or nframes"<<endl;
		abort();
	}

	Disk disk("myvirtualdisk", npages);
    Program my_program;
	
	Page_Replacement page_fault_manager(algorithm, &disk);
    Page_Table pt(npages, nframes, Page_Replacement::page_fault_handler);
	
	unsigned char *virtmem = (unsigned char *) pt.page_table_get_virtmem();
	
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

    pt.page_table_delete();
	disk.close_disk();

	page_fault_manager.print_statistics();
	page_fault_manager.get_instance()->page_replacement_delete();

	return 0;
}