#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
typedef const char* V;
typedef std::vector<V> Record;
typedef std::vector<Record> Page;

#define REG_SIZE 10

int fixed_len_sizeof(Record *record){ 
        //return  (REG_SIZE * record->size());
    int length = 0;
    for (int i = 0; i < record.size(); i ++)
    {
        length = length + sizeof(record[i]);
    }
    return length;
}

void fixed_len_write(Record *record, void *buf) {
    char *buf_return = new char[fixed_len_sizeof(record)];
    for(Record::iterator i = record->begin(); i != record->end(); i++) {
        //printf("%s -- sizeof: %ld\n", *i, sizeof(**i));
        memcpy(buf, *i, REG_SIZE);
        //printf("%s -- sizeof: %ld\n", (unsigned char *) buf, sizeof(buf));
        //memcpy(buf_return, (char *) buf, REG_SIZE);
        //printf("%s -- sizeof: %ld\n", buf_return, sizeof(buf_return));
        buf = (char *) buf + REG_SIZE;
        //printf("%s -- sizeof: %ld\n", (char *) buf-10, sizeof(buf));
        //printf("times");
    }
    
}

void fixed_len_read(void *buf, int size, Record *record) {
    assert(size == fixed_len_sizeof(record));
    for(int i = 0 ; i < REG_SIZE; i++) {
        memcpy((char *) record->at(i), (char *) buf + record->size()*i,REG_SIZE);
    }
    
}

int main() {
    
    Page *page = new Page();
    Record *record1 = new Record();
    Record *record2 = new Record();
    
    record1->push_back((V) "111222\0");
    record2->push_back((V) "333444\0");
    page->push_back(*record1);
    page->push_back(*record2);
    
    long int page_size = 0;    
    for(Page::iterator i = page->begin(); i != page->end(); i++) {
        //printf("Record data: %c\n", (const char *) *(*i));
        page_size += fixed_len_sizeof(*(*i));
    }
    printf("Page size: %ld", page_size);
    
    char *buf = new char[page_size];
    
    //fixed_len_write(record, buf);
    //fixed_len_read(buf,REG_SIZE,record);               
    return 0;
}