#include "heapfile.h"

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record){
   
   int sum = 0; 
   
    for(int i=0 ;i < record->size(); i++){
        if(record->at(i) != NULL)
            sum += strlen(record->at(i));
    }
    printf("fixed_len_sizeof|  %d\n",sum );
    return sum;
}
 
/**
 * Serialize the record to a byte array to be stored in buf.
 */ 
void fixed_len_write(Record *record, void *buf) {
        
    for(std::vector<V>::iterator i = record->begin(); i != record->end(); i++) {
        // printf("fixed_len_write | buf: %s\n",(char*) *i);
        if(*i == NULL)
            return;
        memcpy(buf, *i, REG_SIZE);
        buf = (char *) buf + REG_SIZE;
    }
    //Returning the pointer to the initial position
    // buf = pbuf;
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {

    char* tmp;
    record->reserve(ATTR_TOTAL*REG_SIZE);

    //Checking how many attributs we have in this buf
    int attrQty = size / REG_SIZE;
    // printf("%ld",strlen( (char*) buf));

    //Inicializating values
    for(int i =0; i < attrQty; i++){
        V *tmp2 = new V[REG_SIZE];
        *tmp2 = NULL;
        record->push_back(*tmp2); 
        
    }
    
    std::vector<V>::iterator it;
    it = record->begin();
    
    for(int i = 0; i < attrQty; i++) {
           tmp = new char[size];
           memcpy(tmp, (char *) buf + REG_SIZE*i,REG_SIZE);    
           record->at(i) = (V)tmp;
    }
}

//Inicializates a new page
void _new_page(Page *page){

    int slotsQty = page->page_size / page->slot_size;
       
    page->data = calloc(sizeof(char),page->page_size);
    char *data = (char*) page->data;
    for(int i = 0; i < page->page_size*sizeof(char)  ; i++)
        data[i] = '\0';
}

void init_fixed_len_page(Page*& page, int page_size, int slot_size){
    (page) = new Page;
    (page)->page_size = page_size;
    (page)->slot_size = slot_size;
   
    //Creating a new page
    _new_page(page);
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page){
    return page->page_size/page->slot_size;    
}

// This finds the position of next free slot
int _findNextPageSlot(Page *& page){
    //capacity of the page
    int capacity = fixed_len_page_capacity(page);

    char * data = (char *) page->data;
    for(int i =0 ; i < (capacity * page->slot_size);i++){
        if(data[i] == '\0')
            return i;
    }
    return -1;
}

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page*& page){
        
    int totalSlots = fixed_len_page_capacity(page);
    
    int occupiedSlots = strlen((char*)page->data) / page->slot_size;
    
    return totalSlots - occupiedSlots;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page*& page, Record *r){
   //Check is there is space in this page
    if(fixed_len_page_freeslots(page) < 1){
        return -1;       
    }

    //Create buf and allocat space for it
    void* buf;
    buf =  calloc(sizeof(char),fixed_len_sizeof(r)+1);

    char * data = (char*) page->data;

    //Copy the record to the buffer
    fixed_len_write(r,buf);


    int nextSlot = _findNextPageSlot(page);
    //offseting the pointer to the slot position

    //copy the buf to the page in the slot available
    memcpy(data+nextSlot, buf,strlen((char*)buf));
    
    // printf("next free Slot: %d\n",nextSlot);
    // printf("data: %s\n", data );
    // exit(1);

    //Dealocate spage of buf
    free(buf);

    return nextSlot; 
}

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page*& page, int slot, Record *r){
    
    //Create buf and allocat space for it
    void *buf;
    buf = calloc(sizeof(char),fixed_len_sizeof(r)+1);
    
    char * data = (char*) page->data;

    //Copy the record to the buffer
    fixed_len_write(r,buf);


    int nextSlot = slot * page->slot_size;
    //offseting the pointer to the slot position

    //copy the buf to the page in the slot available
    memcpy(data+nextSlot, buf,strlen((char*)buf));
    
    // printf("next  Slot: %d\n",nextSlot);
    // printf("data: %s\n", data );
    // exit(1);
}

/**
 * Read a record from the page from a given slot.
 */
int read_fixed_len_page(Page *page, int slot, Record *& r){

    char * buf;
    buf = (char*) calloc(sizeof(char),REG_SIZE*ATTR_TOTAL+1);
    
    char * data = (char*) page->data;

    int posPage = page->slot_size * slot;

    memcpy(buf, data+posPage,page->slot_size);
    
    // printf("read_fixed_len_page 1| posPage: %d slot %d slot_size %d\n",posPage,slot,page->slot_size);
    // printf("read_fixed_len_page 2| buf %s \n", data );

    fixed_len_read(buf,strlen(buf),r);

    free(buf);
    return 0;
}


//The first thing is find how many pages a Heap page fits
//Using 1 int to store the page link
// And 1 int to store the pID and 1 int to store the freespace
int _nPages(Heapfile *heapfile){
    return (heapfile->page_size - 1 * sizeof(int))/(2 * sizeof(int)); //Number of pages
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

void _getHeapPageId(Heapfile* heapfile,int offset){
    FILE * file = heapfile->file_ptr;
    int value = -1;
    int nPages = _nPages(heapfile);
    offset = offset*(nPages+1)*heapfile->page_size;
    fseek(file,offset,SEEK_SET);
    fread(&value,sizeof(int),1,file);
    printf("_getHeapPageId %d\n", value);
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
    if((lastPageId+1) > 0){
        offset = (lastPageId)*(nPages+1)*heapfile->page_size;
        fseek(file,offset,SEEK_SET);
        lastPageId++;
        fwrite(&lastPageId,sizeof(int),1,file);
        lastPageId--;
        printf("_new_heapfile|lastHeapPageId: %d, nPages %d, offset %d,file_init %d, heapfile->page_size %d\n", lastPageId+1,nPages,offset,file_init, heapfile->page_size);
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
    
    // printf("init_heapfile|page-size %d\n", page_size);
    //build the heapfile
    _new_heapfile(heapfile);

    //Go to the file begining and write 0 to show that is no other page
    rewind(file);    

}

// This finds the next available slot in the heapfile
// íf there is no slot returns -1;
// or the HeapPageId if sucess
int _nextSlot(Heapfile *heapfile,int *_heapPageId){
    FILE * file = heapfile->file_ptr;
    int nPages = _nPages(heapfile);//# of pages slots inside a Heap Page
    int heapPageId = 0;
    int pageId = 0;
    int freeSpace = 0;

    rewind(file);
    do{
        //This is the last heapPageId there is not 0
        *_heapPageId = heapPageId; 
        // printf("heapPageId2 %d\n",*_heapPageId );
        
        //Get the first heapPageId
        fread(&heapPageId,sizeof(int),1,file); 
        //Go trough a Heap Page
        for(int j=0;j<nPages;j++){
            fread(&pageId,sizeof(int),1,file); 
            fread(&freeSpace,sizeof(int),1,file); 
            if(freeSpace == heapfile->page_size)
                return pageId;
        }
               // _getHeapPageId(heapfile,heapPageId); 
             // printf("_nextSlot| _heapPageId %d heapPageId %d freeSpace: %d\n",*_heapPageId,heapPageId,freeSpace);

        //Go the next Heap Page
        fseek(file, heapfile->page_size*heapPageId*(nPages+1),SEEK_SET);
    }while(heapPageId != 0);
    
    return -1;
}





PageID alloc_page(Heapfile *heapfile){

    FILE * file = heapfile->file_ptr;
    //Fist we need to get the HeapPageId of the last page;
    int *nextHeapId =  new int();
    int nextSlot = _nextSlot(heapfile,nextHeapId);
    //Check if there is any slot available on the Heapfile
    // printf("Alloc|nextSlot:%d\n", nextSlot);    
    if(nextSlot != -1)
        return nextSlot;
    //There isnt any free slot 
    // We need to create a new HeapFile Page
    _new_heapfile(heapfile);

    // We should have a free slot now
    return _nextSlot(heapfile,nextHeapId);

}


/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page){
    FILE * file = heapfile->file_ptr;

    int offset;
    int heapPageId;
    int nPages = _nPages(heapfile);
    int pagePos; //Page position inside of a heap file page
    void *data = calloc(sizeof(int),heapfile->page_size);
    
    //We need to go to the page pid offset
    //1 - Find the HeapPageId of this pid
    int pageSlotOffset = (((int) pid)/ nPages) + ((int) pid) + 1;
    
    // 2 - Find the position of the page inside a Heap Page
    pagePos = (( int ) pid) / nPages;
    offset =  pageSlotOffset * heapfile->page_size ;//+  ((pagePos)*heapfile->page_size);
    // offset = pageSlotOffset*heapfile->page_size;

    // GO to the offset position and retrieve the data
    fseek(file,offset,SEEK_SET);
    fread(data,sizeof(char),heapfile->page_size,file);

    //setting the data
    page->data = (void*) data;
    // printf("read_page| pid %d offset %d pageSlotOffset %d pagePos %d page-data %s\n",
        // pid,offset,pageSlotOffset,pagePos,data);

}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid){
    FILE * file = heapfile->file_ptr;

    int offset;
    int heapPageId;
    int slotPid;
    int nPages = _nPages(heapfile);
    int _pid = pid;
    int freeSpace = fixed_len_page_freeslots(page) * page->slot_size;
    

    // We need to write the pid in the slot and set the new space;

    //We need to go to the page pid offset
    //1 - Find the HeapPageId of this pid
    heapPageId = (( int ) pid) / nPages;
    //2 We need to find the slot of the pid within the HeapPageId
    slotPid = (int) pid % nPages;

    offset = (nPages+1)*heapPageId * heapfile->page_size + (slotPid)*( sizeof(int)*2) + sizeof(int);
    fseek(file,offset,SEEK_SET);
    fwrite(&_pid,sizeof(int),1,file);
    fwrite(&freeSpace,sizeof(int),1,file);
    // Find the right slot position to write the page
    int pageSlotOffset = (((int) pid)/ nPages) + ((int) pid) + 1;

    // GO to the offset position and write the data
    offset = pageSlotOffset*heapfile->page_size;
    // printf("write_page|pid %d heapPageId %d ofsset %d freeSpace %d heapfile->size %d\n",
     // pid,heapPageId,offset,freeSpace,heapfile->page_size );
    fseek(file,offset,SEEK_SET);
    fwrite(page->data,sizeof(char),heapfile->page_size,file);

}



RecordIterator::RecordIterator(Heapfile *heapfile){
    
    // Initializating variables
    // this->heapfile = new Heapfile();
    this->record = new Record();
    this->page = new Page();
    this->heapfile = heapfile;
    this->currentPage = 0;
    this->currentSlot = 0;
    


    // printf("RecordIterator| heap->p_Size %d\n",this->heapfile->page_size );
    init_fixed_len_page(this->page,this->heapfile->page_size,ATTR_TOTAL*REG_SIZE);//TODO Change 20 to ATTR_TOTAL after
    this->nSlots = fixed_len_page_capacity(this->page);
    
    // TODO FIX THIS NUMBER
    this->nPages = (_HeapLastPage(this->heapfile)+1);
    printf("RecordIterator::RecordIterator| _HeapLastPage  %d _nPages  %d\n",
     _HeapLastPage(this->heapfile)+1,_nPages(heapfile));
   
    //Read the first page of the HeapFile
    read_page(this->heapfile,this->currentPage,this->page);
}



Record  RecordIterator::next(){
    

    read_fixed_len_page(page,currentSlot,this->record); 
    if((currentSlot >= (nSlots-1)) && hasNext()){
        currentPage++;
        read_page(this->heapfile,this->currentPage,this->page);
        this->nSlots = fixed_len_page_capacity(this->page);
        printf("RecordIterator::next| currentSlot %d currentPage %d  nSlots %d  nPages %d\n",this->currentSlot,currentPage,nSlots,nPages );

        this->currentSlot = 0;
        return *this->record;
    }

    this->currentSlot++;
    return *this->record;


}
bool RecordIterator::hasNext(){

    int heapPageId=0;
    int nextFreePid = _nextSlot(heapfile,&heapPageId);
    heapPageId++;
    int len_data;

    len_data = ATTR_TOTAL * REG_SIZE*currentSlot;
    if(nextFreePid != -1){
        if(strlen((char*)page->data)>len_data) 
            return true;
        
    }

    if(currentPage <= nextFreePid){
    printf("RecordIterator::hasNext|nPages %d heapPageId %d nextFreePid %d currentPage %d data_size %d len_data %d\n",
        nPages,heapPageId,nextFreePid,currentPage,strlen((char*)page->data),len_data);
         // currentPage++;
        if((currentPage+1) < nPages)
            return true;
    }

    return false;
}


 /*
 int main() {
   
     Page *page = new Page();
     Page *page2 = new Page();
     Record *record1 = new Record();
     Record *record2 = new Record();
     Record *record3 = new Record();
     Record *record4 = new Record();
    
     record1->push_back((V) "5111222333\0");
     record1->push_back((V) "A111222333\0");
     record2->push_back((V) "6111222333\0");
     record2->push_back((V) "B111222333\0");
     record3->push_back((V) "LUCASLUCAS\0");
     record3->push_back((V) "C111222333\0");
     record4->push_back((V) "8111222333\0");
     record4->push_back((V) "D111222333\0");
  
     init_fixed_len_page(page,40,ATTR_TOTAL);
     add_fixed_len_page(page, record1);
     add_fixed_len_page(page, record2);

     init_fixed_len_page(page2,40,ATTR_TOTAL);
    

     write_fixed_len_page(page,0,record3);    
     // printf("record3 %s\n",record3->at(0));
     read_fixed_len_page(page,1,record4);

    
     FILE * file;
     int pageId;
     file = fopen("txt_files/test.txt","wb+");
     Heapfile * heapfile = new Heapfile;
     init_heapfile(heapfile,40,file);

//     //adding 30 pages to the heap file
     for(int i = 0 ; i < 30; i++){    
         pageId = (int) alloc_page(heapfile);
         // printf("main|i %d  alloc_page: %d\n",i,pageId);
         write_page(page, heapfile, pageId);
     }


     // write_fixed_len_page(page2,0,record3);   
     read_page(heapfile, 0 ,page2);
    
     Record * new_record  = new Record();
     read_fixed_len_page(page2,0,new_record); 

     // long int page_size = 0;    
    
     char *buf = new char[41*sizeof(char)];
    
    RecordIterator *iterator = new RecordIterator(heapfile);
    *new_record = iterator->next();
    *new_record = iterator->next();
    *new_record = iterator->next();
    *new_record = iterator->next();
    *new_record = iterator->next();
     fixed_len_write(new_record, buf);
     // fixed_len_read(buf,stlen(buf),new_record);
        
     printf("main| buffer: %s\n",buf);


     // std::vector<V>::iterator it;
     // for (it=record2->begin(); it<record2->end(); it++)
     // std::cout << ' ' << *it;
    
     return 0;
}
*/