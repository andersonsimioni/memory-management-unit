#include "disk.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/*
 * NÃO MODIFICAR ESSE ARQUIVO!
 */

Disk::Disk(const char *diskname, int blocks)
{
	fd = open(diskname, O_CREAT | O_RDWR,0777);

    if(fd < 0) {
		cout << "Error when opening the file " << diskname << "\n";
		return;
	}

    nblocks = blocks;
	
    if(ftruncate(fd, nblocks * DISK_BLOCK_SIZE) < 0) {
        close(fd);
    }
}

int Disk::get_nblocks()
{
	return nblocks;
}

void Disk::sanity_check( int blocknum, const void *data )
{
	if(blocknum < 0) {
		cout << "DISK ERROR: blocknum (" << blocknum << ") is negative!\n";
		abort();
	}

	if(blocknum >= nblocks) {
		cout << "DISK ERROR: blocknum (" << blocknum << ") is too big!\n";
		abort();
	}

	if(!data) {
		cout << "DISK ERROR: null data pointer!\n";
		abort();
	}
}

void Disk::read(int blocknum, char *data )
{
	sanity_check(blocknum, data);

    int actual = pread(fd, data, DISK_BLOCK_SIZE, blocknum * DISK_BLOCK_SIZE);

    if(actual != DISK_BLOCK_SIZE) {
		cout << "DISK READ ERROR: failed to read block " << blocknum << " errno = " << strerror(errno) << endl;
		abort();
	}
}

void Disk::write(int blocknum, const char *data)
{
	sanity_check(blocknum, data);

    int actual = pwrite(fd, data, DISK_BLOCK_SIZE, blocknum * DISK_BLOCK_SIZE);
	
    if(actual != DISK_BLOCK_SIZE) {
		cout << "DISK WRITE ERROR: failed to write block " << blocknum << " errno = " << strerror(errno) << endl;
		abort();
	}
	
}

void Disk::close_disk()
{
	close(fd);
	fd = 0;
}

