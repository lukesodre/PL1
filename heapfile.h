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

int fixed_len_sizeof(Record *record);
void fixed_len_write(Record *record, void *buf) ;
void fixed_len_read(void *buf, int size, Record *record);
void init_fixed_len_page(Page*& page, int page_size, int slot_size);
int fixed_len_page_capacity(Page *page);
int fixed_len_page_freeslots(Page*& page);
int add_fixed_len_page(Page*& page, Record *r);
int write_fixed_len_page(Page*& page, int slot, Record *r);
int read_fixed_len_page(Page *page, int slot, Record *& r);
int _nPages(Heapfile *heapfile);
int _HeapLastPage(Heapfile *heapfile);
void _new_heapfile(Heapfile *heapfile);
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);
int _nextSlot(Heapfile *heapfile,int * _heapPageId);
PageID alloc_page(Heapfile *heapfile);




