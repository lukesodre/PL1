#include "heapfile.h"

int fixed_len_sizeof(Record *record){
   int sum = 0; 
   
   for(int i=0;i<record->size();i++)
       sum += strlen(record->at(i));
   
   return   sum;
}
 
void fixed_len_write(Record *record, void *buf) {
    char *buf_return = new char[fixed_len_sizeof(record)];
    for(std::vector<V>::iterator i = record->begin(); i != record->end(); i++) {
        //printf("%s -- sizeof: %ld\n", *i, sizof(**i));
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


//The first thing is find how many pages a Heap page fits
//Using 1 int to store the page link
// And 1 int to store the pID and 1 int to store the freespace
int _nPages(Heapfile *heapfile){
    return (heapfile->page_size - 1*sizeof(int))/(2 * sizeof(int)); //Number of pages
}
//This function seet the file pointer to the last Heap File page 
// and return the HeapPageId of the last non 0
int _HeapLastPage(Heapfile *heapfile){
    FILE * file = heapfile->file_ptr;
    rewind(file);
    int heapPageId, lastHeapPageId;
    int nPages = _nPages(heapfile);

    heapPageId = 0;
    lastHeapPageId = 0;
    fread(&lastHeapPageId,sizeof(int),1,file); 
    //Check if the Heap file has been already created;
    if(-1 == lastHeapPageId){
        return -1;
    }
    
    
    do{  
        lastHeapPageId = heapPageId;
        //page_size * ofsset * (pages_of_records + page of heap) 
        fseek(file, heapfile->page_size*heapPageId*(nPages + 1), SEEK_SET);  
        fread(&heapPageId,sizeof(int),1,file); 
        
    }while(heapPageId != 0);


    return lastHeapPageId;
}

//This is used to set the slots and set the free space
void _new_heapfile(Heapfile *heapfile){
    FILE * file = heapfile->file_ptr;

    //The first element is the link to other heap page
    //0 means that is not such page
    int file_init = 0;
    int nPages = _nPages(heapfile);
    int lastPageId = _HeapLastPage(heapfile);
    int offset = 0;

    //Update the link of the last Heap Page;
    if(lastPageId > 0){
        offset = (lastPageId+1)*(nPages+1)*heapfile->page_size;
        fseek(file,offset,SEEK_SET);
        lastPageId++;
        fwrite(&lastPageId,sizeof(int),1,file);
    }


    //set the Offset the new page
    offset = (lastPageId+1)*(nPages+1)*heapfile->page_size;
    fseek(file,offset,SEEK_SET);
    
    //Set the pointer after the heapPageId
    fwrite(&file_init,sizeof(int),1,file);
    
    //Inicializates the page with the blank values
    for(int i = 0; i < nPages; i++){
        file_init = (lastPageId+1)*(nPages)+i;
        fwrite(&file_init,sizeof(int),1,file);
        fwrite(&heapfile->page_size,sizeof(int),1,file);
    }

}

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){

    heapfile->page_size = page_size;
    heapfile->file_ptr = file;

    //Set the flag saying that the heap file dont exits yet;
    rewind(file);
    int file_init = -1;
    fwrite(&file_init,sizeof(int),1,file);
    
    //build the heapfile
    _new_heapfile(heapfile);

    //Go to the file begining and write 0 to show that is no other page
    rewind(file);    

}

// This finds the next available slot in the heapfile
// íf there is no slot returns -1;
// or the HeapPageId if sucess
int _nextSlot(Heapfile *heapfile,int * _heapPageId){
    FILE * file = heapfile->file_ptr;
    int nPages = _nPages(heapfile);//# of pages slots inside a Heap Page
    int heapPageId = 0;
    int pageId = 0;
    int freeSpace = 0;

    rewind(file);
    do{
        //This is the last heapPageId there is not 0
        *_heapPageId = heapPageId; 
        
        //Get the first heapPageId
        fread(&heapPageId,sizeof(int),1,file); 

        //Go trough a Heap Page
        for(int j=0;j<nPages;j++){
            fread(&pageId,sizeof(int),1,file); 
            fread(&freeSpace,sizeof(int),1,file); 
            
            if(freeSpace == heapfile->page_size)
                return pageId;
        }

        //Go the next Heap Page
        fseek(file, heapfile->page_size*heapPageId*(nPages+1)*heapPageId,SEEK_SET);
    }while(heapPageId != 0);
    
    return -1;
}





PageID alloc_page(Heapfile *heapfile){

    FILE * file = heapfile->file_ptr;
    //Fist we need to get the HeapPageId of the last page;
    int *nextHeapId;
    int nextSlot = _nextSlot(heapfile,nextHeapId);
    //Check if there is any slot available on the Heapfile
    if(nextSlot != -1)
        return nextSlot;
    
    //There isnt any free slot 
    // We need to create a new HeapFile Page
    _new_heapfile(heapfile);

    // We should have a free slot now
    return _nextSlot(heapfile,nextHeapId);

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