#ifndef DISK_H
#define DISK_H

/*
 * NÃO MODIFICAR ESSE ARQUIVO!
 */

#include <fstream>
#include <iostream>

using namespace std;

class Disk
{
public:
    static const unsigned short int DISK_BLOCK_SIZE = 4096;
    static const unsigned int DISK_MAGIC = 0xdeadbeef;

    /*
    * Create a new virtual disk in the file "filename", with the given number of blocks.
    */
    Disk(const char *filename, int nblocks);

    /*
    * Read exactly BLOCK_SIZE bytes from a given block on the virtual disk.
    * "block" is the block number and "data" is a pointer to where the data will be placed.
    */
    void read(int blocknum, char * data);

    /*
    * Write exactly BLOCK_SIZE bytes to a given block on the virtual disk.
    * "d" must be a pointer to a virtual disk, "block" is the block number,
    * and "data" is a pointer to the data to write.
    */
    void write(int blocknum, const char * data);

    /* Return the number of blocks in the virtual disk. */
    int get_nblocks(); 

    /* Closes the virtual disk. */
    void close_disk();

private:
    void sanity_check(int blocknum, const void *data);

private:
    int fd;
    int nblocks;
};


#endif