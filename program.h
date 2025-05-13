#ifndef PROGRAM_H
#define PROGRAM_H

/*
 * NÃO MODIFICAR ESSE ARQUIVO!
 */

#include <iostream>

using namespace std;

class Program 
{
public:
    void alpha( unsigned char *data, int length);
    void beta( unsigned char *data, int length);
    void gamma( unsigned char *data, int length);
    void delta( unsigned char *data, int length);
private:
    static int compare_bytes( const void *pa, const void *pb )
    {
        unsigned char a = *(unsigned char*)pa;
        unsigned char b = *(unsigned char*)pb;

        if(a<b) {
            return -1;
        } else if(a==b) {
            return 0;
        } else {
            return 1;
        }

    }
};


#endif
