#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#define TRUE 0
#define FALSE 1
#define HEAP_FILE_FLAG TRUE

// Record
typedef const char* V;
typedef std::vector<V> Record;
typedef std::vector<Record*> Slots;

// RecordID
typedef struct {
    int page_id;
    int slot;
} RecordID;

// Page
typedef struct {
    void *data;
    int page_size;
    int slot_size;
} Page;

// PageID
typedef int PageID;

// Heapfile
typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;

/*<<======Heapfile methods======>>*/

// Initalize a heapfile to use the file and page size given.
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/*<<======Page methods======>>*/

// Allocate another page in the heapfile. This grows the file by a page.
PageID alloc_page(Heapfile *heapfile);

// Calculate the free space (number of free slots) in the page
int fixed_len_page_freeslots(Page *page);

// Read a page into memory
void read_page(Heapfile *heapfile, PageID pid, Page *page);

// Write a page from memory to disk
void write_page(Page *page, Heapfile *heapfile, PageID pid);

// Initiate Page with fixed length
void init_fixed_len_page(Page *page, int page_size, int slot_size);

// Calculate the maximal number of records that fit in a page
int fixed_len_page_capacity(Page *page);

/*<<======Record methods======>>*/

// Compute the number of bytes required to serialize record 
int fixed_len_sizeof(Record *record);

/**
 * Deserializes 'size' bytes from the buffer 'buf', and
 * stores the record in 'record'.
 */ 
void fixed_len_read(void *buf, int size, Record *record);

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);

// Write a record into a given slot.
void write_fixed_len_page(Page *page, int slot, Record *r);

// Read a record from the page from a given slot.
// (Daniel) Non sense
void read_fixed_len_page(Page *page, int slot, Record *r);