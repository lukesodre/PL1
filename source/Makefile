# Makefile
CC = g++ 
 
heapfile.o: heapfile.cpp heapfile.h
	$(CC) -o $@ -c $<
 
csv2heapfile: csv2heapfile.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
scan: scan.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
insert: insert.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
update: update.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
delete: delete.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
select: select.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
csv2colstore: csv2colstore.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
select2: select2.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o
 
select3: select3.cpp heapfile.o
	$(CC) -o $@ $< heapfile.o