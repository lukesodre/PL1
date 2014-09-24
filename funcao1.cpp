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

#define REG_SIZE 10
#define ATTR_TOTAL 100

int fixed_len_sizeof(Record *record){
//         printf("length: %d\n",strlen(record->at(0)));
        return   strlen(record->at(0));
}

void fixed_len_write(Record *record, void *buf) {
    char *buf_return = new char[fixed_len_sizeof(record)];
    for(std::vector<V>::iterator i = record->begin(); i != record->end(); i++) {
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

    char* tmp;
    record->reserve(ATTR_TOTAL);
    
    //Inicializating values
    for(int i =0 ;i < ATTR_TOTAL;i++){
        V *tmp2 = new V[REG_SIZE];
        record->push_back(*tmp2); 
        
    }
    
    std::vector<V>::iterator it;
    it = record->begin();
    
    for(int i = 0 ; i < 2; i++) {
           tmp = new char[size];
           memcpy(tmp, (char *) buf + 2*i,REG_SIZE);    
           record->at(i) = (V)tmp;
    }
    
    
}


void init_fixed_len_page(Page*& page, int page_size, int slot_size){
    (page) = new Page;
    (page)->page_size = page_size;
    (page)->slot_size = slot_size;
    
    //Vector of records 
    Slots * slots = new Slots[page_size/slot_size];
//     (page)->data = new Slots[page_size/slot_size];
    
    //Each page has a vector of records 
    (page)->data = slots;
    
}

int fixed_len_page_capacity(Page *page){
     return page->page_size/page->slot_size;    
}

int fixed_len_page_freeslots(Page*& page){
        Slots *slots;
         
        slots = (Slots*) page->data;
        return  fixed_len_page_capacity(page) - slots->size();
        
 
}



int add_fixed_len_page(Page*& page, Record *r){
   if(fixed_len_page_freeslots(page) < 1){
     return -1; //record slot offset if successful      
    }
    
    
    //Retreving the slots
    Slots *a;
    a = (Slots*) page->data;
    
    //Adding the record in the slot
    a->push_back(r);
    
    //Return the offset
    return a->size() -1;    
}

int write_fixed_len_page(Page*& page, int slot, Record *r){
    //Check if the slot is within the page range
    if(slot > fixed_len_page_freeslots(page))
        return -1;
   //Retreving the slots
   Slots *a;
   a = (Slots*) page->data;
   
   //Add the record in the slot slot;
   if(slot < a->size())
       a->at(slot) = r; 
   else
       return -1;
   
}


int read_fixed_len_page(Page *page, int slot, Record *& r){

    //Check if the slot is within the page range
    if(slot > fixed_len_page_freeslots(page))
        return -1;

   //Retreving the slots
   Slots *a;
   a = (Slots*) page->data;
   
   //save the record in record
   r = a->at(slot); 
}

int main() {
    
    Page *page = new Page();
    Record *record1 = new Record();
    Record *record2 = new Record();
    Record *record3 = new Record();
    Record *record4 = new Record();
    
    record1->push_back((V) "5111222333\0");
    record2->push_back((V) "6111222333\0");
    record3->push_back((V) "7111222333\0");
    record4->push_back((V) "8111222333\0");
    
    init_fixed_len_page(page,20,2);
    
    add_fixed_len_page(page, record1);
    add_fixed_len_page(page, record2);
    
    write_fixed_len_page(page,0,record3);    
    read_fixed_len_page(page,1,record4);
    
    Slots *a;
    a = (Slots*)page->data;
    printf("%s\n",record4->at(0));     
    

    long int page_size = 0;    
    
    char *buf = new char[page_size];
    
    fixed_len_write(record1, buf);
    fixed_len_read(buf,20,record2);
        
//     printf("%s\n",buf
    std::vector<V>::iterator it;
    for (it=record2->begin(); it<record2->end(); it++)
    std::cout << ' ' << *it;
    return 0;
}