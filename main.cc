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
	const char *program = argv[4];

	Disk disk("myvirtualdisk", npages);
    Program my_program;

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
	return 0;
}