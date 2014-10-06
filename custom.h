#define TRUE 0
#define FALSE 1

#ifndef HEAP_FILE_FLAG
	#include "heapfile.h"
	#define HEAP_FILE_FLAG TRUE
#endif

#define DEBUG TRUE
#define REG_SIZE 10
#define ATTR_TOTAL 100

// Return the number of pages in a Heapfile
int number_of_pages(Heapfile *heapfile);

// Return last page in the Heapfile
int last_heapfile_page(Heapfile *heapfile);

// Finds next slot within a Heapfile
int find_next_available_heapfile_slot(Heapfile *heapfile, int *heapPageId);

// Finds the next free slot position within a Page
int find_next_available_slot(Page *page);

// Serialize the record to a byte array to be stored in buf.
int fixed_len_write(Record *record, void *buf);

void update_heapfile(Heapfile *heapfile);