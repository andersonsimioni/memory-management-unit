#include "program.h"
#include <stdlib.h>

/*
 * NÃO MODIFICAR ESSE ARQUIVO!
 */

void Program::alpha( unsigned char *data, int length )
{
	unsigned long total = 0;
	int i, j;

	cout << "alpha: initializing memory\n";

	for(i = 0; i < length; i++) {
		data[i] = 0;
	}

	cout << "alpha: modifying memory...\n";

	for(j = 0; j < 100; j++) {
		int start = j * 4096;
		int chunksize = 10 * 4096;

		for(i = 0;i < chunksize; i++) {
			data[ (start+i) % length ] = i;
		}
	}

	cout << "alpha: summing up results\n";

	for(i = 0; i < length; i++) {
		total += data[i];
	}

	cout << "alpha: result is " << total << endl;
}

void Program::beta( unsigned char *data, int length )
{
	unsigned total = 0;
	int i;

	srand48(4856);

	cout << "beta: initializing memory\n";

	for(i = 0; i < length; i++) {
		data[i] = lrand48();
	}

	cout << "beta: sorting memory\n";
	
    qsort(data, length, 1, Program::compare_bytes);

	cout << "beta: adding up all values\n";

	for(i = 0; i < length; i++) {
		total += data[i];
	}

    cout << "beta: result is " << total << endl;
}

void Program::gamma( unsigned char *data, int length )
{
	int i, j;
	unsigned char *a = data;
	unsigned char *b = &data[length / 2];
	unsigned total = 0;

	cout << "gamma: initializing two large vectors\n";

	for(i = 0; i < length / 2; i++) {
		a[i] = i % 256;
		b[i] = i % 171;
	}

	for(j = 0; j < 10; j++) {
		cout << "gamma: computing dot product\n";
		for(i = 0; i < length / 2; i++) {
			total += a[i] * b[i];
		}
	}

    cout << "gamma: result is " << total << endl;
}

void Program::delta( unsigned char *data, int length )
{
	int i;
	unsigned total = 0;

	srand48(30341);

	cout << "delta: initializing memory\n";
	for(i = 0; i < length; i++) {
		data[i] = i * 2;
	}

	cout << "delta: swapping random locations\n";
	for(i = 0;i < 100000; i++) {
		int a = lrand48() % length;
		int b = lrand48() % length;
		int t = data[a];
		data[a] = data[b];
		data[b] = t;
	}

	cout << "delta: adding up all values\n";
	for(i = 0;i < length; i++) {
		total += data[i];
	}

    cout << "delta: result is " << total << endl;
}
