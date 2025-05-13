virtmem: main.o page_table.o disk.o program.o page_replacement.o
	g++ main.o page_table.o disk.o program.o page_replacement.o -o virtmem -g

main.o: main.cc
	g++ -Wall -g -c main.cc -o main.o

page_table.o: page_table.cc
	g++ -Wall -g -c page_table.cc -o page_table.o

page_replacement.o: page_replacement.cc
	g++ -Wall -g -c page_replacement.cc -o page_replacement.o

disk.o: disk.cc
	g++ -Wall -g -c disk.cc -o disk.o

program.o: program.cc
	g++ -Wall -g -c program.cc -o program.o

clean:
	rm -f *.o virtmem
