#include "page_table.h"

/*
 * NÃO MODIFICAR ESSE ARQUIVO!
 */

Page_Table *Page_Table::the_page_table;

Page_Table::Page_Table(int npages, int nframes, Page_Table::page_fault_handler_t handler)
{
    int i;
	char filename[256];

	the_page_table = this;

	sprintf(filename, "/tmp/pmem.%d.%d", getpid(), getuid());

	fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0777);
	
    if(!fd) {
        cout << "Page Table ERROR: it was not possible to create the page table file in the filesystem.\n";
        return;
    }

	ftruncate(fd, PAGE_SIZE * npages);

	unlink(filename);

	this->physmem = (char *) mmap(0, nframes * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	this->nframes = nframes;

	this->virtmem = (char *) mmap(0, npages * PAGE_SIZE, PROT_NONE, MAP_SHARED | MAP_NORESERVE, fd, 0);
	this->npages = npages;

	this->page_bits = new int(sizeof(int) * npages);
	this->page_mapping = new int(sizeof(int) * npages);

	this->handler = handler;

	for(i = 0; i < this->npages; i++) 
    this->page_bits[i] = 0;

	this->sa.sa_sigaction = Page_Table::internal_fault_handler;
	this->sa.sa_flags = SA_SIGINFO;

	sigfillset( &this->sa.sa_mask );
	sigaction( SIGSEGV, &this->sa, 0 );
}

void Page_Table::page_table_delete()
{
	munmap(virtmem, npages * PAGE_SIZE);
	munmap(physmem, nframes * PAGE_SIZE);
	delete page_bits;
	delete page_mapping;
	close(fd);
}

void Page_Table::page_table_set_entry(int page, int frame, int bits)
{
	if( page < 0 || page >= npages ) {
		cout << "page_table_set_entry: illegal page" << page << endl;
		abort();
	}

	if( frame < 0 || frame >= nframes ) {
		cout << "page_table_set_entry: illegal frame " << frame << endl;
		abort();
	}

	page_mapping[page] = frame;
	page_bits[page] = bits;

	remap_file_pages(virtmem + page * PAGE_SIZE, PAGE_SIZE, 0, frame, 0);
	mprotect(virtmem + page * PAGE_SIZE, PAGE_SIZE, bits);
}

void Page_Table::page_table_get_entry(int page, int *frame, int *bits)
{
	if( page < 0 || page >= npages) {
		cout << "page_table_get_entry: illegal page " << page << endl;
		abort();
	}

	*frame = page_mapping[page];
	*bits = page_bits[page];
}

void Page_Table::page_table_print_entry(int page)
{
	if( page < 0 || page >= npages ) {
		cout << "page_table_print_entry: illegal page " << page;
		abort();
	}

	int b = page_bits[page];

    cout << "page " << page << "frame " << page_mapping[page] << " bits " << 
    (b & PROT_READ  ? 'r' : '-') << 
    (b & PROT_WRITE ? 'w' : '-') <<
    (b & PROT_EXEC  ? 'x' : '-') << endl;
}

void Page_Table::page_table_print()
{
	int i;
	for(i = 0; i < npages; i++) {
		page_table_print_entry(i);
	}
}

int Page_Table::page_table_get_nframes()
{
	return nframes;
}

int Page_Table::page_table_get_npages()
{
	return npages;
}

void * Page_Table::page_table_get_virtmem()
{
	return virtmem;
}

void * Page_Table::page_table_get_physmem()
{
	return physmem;
}
