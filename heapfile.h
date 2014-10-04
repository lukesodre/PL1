#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
typedef const char* V;
typedef std::vector<V> Record;
typedef std::vector<Record *> Slots;

typedef struct {
    void *data;
    int page_size;
    int slot_size;
} Page;

typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;

typedef int PageID;

typedef struct {
    int page_id;
    int slot;
} RecordID;

#define REG_SIZE 10
#define ATTR_TOTAL 100


/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

/**
 * Serialize the record to a byte array to be stored in buf.
 */
// void fixed_len_write(Record *record, void * buf) ;

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);

void fixed_len_write(Record *record, void * buf);


void _new_page(Page *page);

void init_fixed_len_page(Page*& page, int page_size, int slot_size);

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page*& page);

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page*& page, Record *r);

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page*& page, int slot, Record *r);

int read_fixed_len_page(Page *page, int slot, Record *& r);

int _nPages(Heapfile *heapfile);

int _HeapLastPage(Heapfile *heapfile);

void _new_heapfile(Heapfile *heapfile);

void init_heapfile(Heapfile *, int , FILE *);

int _nextSlot(Heapfile *heapfile,int * _heapPageId);

PageID alloc_page(Heapfile *heapfile);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);


/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

// This finds the position of next free slot
int _findNextPageSlot(Page *& page);




