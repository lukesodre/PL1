#include "heapfile.h"
#include "custom.h"

/**
 * Allocate memory to a new page
 */
void init_fixed_len_page(Page **page, int page_size, int slot_size){
    *page = new Page;
    (*page)->page_size = page_size;
    (*page)->slot_size = slot_size;

    // Initializes all bits to zero.
    // Add 1 to page_size to handle \0 scape sequence at data end
    (*page)->data = calloc((*page)->page_size+1, sizeof(char));

    if (DEBUG == TRUE){
        printf("init_fixed_len_page - Page allocated successfully\n");
        printf("---------------------------\n");
    }
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page **page){
    return (*page)->page_size / (*page)->slot_size;
}

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page **page){
    int total_num_slots = fixed_len_page_capacity(&(*page));
    int occupied_slots = strlen((char*)(*page)->data) / (*page)->slot_size;
    return total_num_slots - occupied_slots;
}

/**
 * Compute the number of bytes required to serialize record 
 */
int fixed_len_sizeof(Record *record){
    int sum = 0; 
    for(int i = 0;i < record->size(); i++){
        sum += strlen(record->at(i));
    }
    return sum;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */ 
int fixed_len_write(Record *record, void *buf) {
       
    /* Created for DEBUG purpose */ 
    int buffer_total_size = 0;

    for(int i = 0; i < record->size(); i++) {
        // sizeof(*record->at(i)) --> returns the actual value of V
        // The +1 is necessary to add a split signal between records
        memcpy(buf, record->at(i), REG_SIZE);
        buf = (char *) buf + REG_SIZE;
        /* Incremented for DEBUG purpose */ 
        buffer_total_size += REG_SIZE;
    }
    // Returns the pointer to the initial position
    buf = (char *) buf - buffer_total_size;
    
    if (DEBUG == TRUE){
        printf("Buffer content: ");
        for(int i = 0; i < buffer_total_size; i++) {
            printf("%c", (*((char *) buf)));
            buf = ((char *) buf + 1);
        }
        printf("\n");
        //Returning the pointer to the initial position
        buf = (char *) buf - buffer_total_size;   
        printf("fixed_len_write - Serialized the record successfully\n");
        printf("---------------------------\n");    
    }

    return buffer_total_size;
}

// Finds the position of the next available slot
signed int find_next_available_slot(Page **page){
    char *data = (char *) (*page)->data;
    for(int i = 0; i < ((*page)->page_size); i++){
        if(*(data+i) == '\0'){
            if(DEBUG == TRUE){
                printf("find_next_available_slot - Found where page data ends successfully\n");
                printf("---------------------------\n");
            }
            return i;
        }
    }

    if(DEBUG == TRUE){
        printf("find_next_available_slot - Could not find where page data ends\n");
        printf("---------------------------\n");
    }
    return -1;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
signed int add_fixed_len_page(Page **page, Record *record){
    //Check if there is any available space in this page
    if(fixed_len_page_freeslots(&(*page)) < 1){
        if (DEBUG == TRUE){
            printf("add_fixed_len_page - No available slots in the page\n");
            printf("---------------------------\n");
        }
        return -1;       
    }

    //Create buf and allocate space for it
    void *buf = calloc(fixed_len_sizeof(record), sizeof(char));

    //Copy the record to the buffer
    int buffer_size = fixed_len_write(record, buf);

    // Finds the position of the next available slot
    signed int next_slot = find_next_available_slot(&(*page));

    // Copy buf to the available page slot
    char *page_data = (char*) (*page)->data;
    memcpy(page_data+next_slot, buf, buffer_size);

    //Deallocate space of buf
    free(buf);

    if (DEBUG == TRUE){
        printf("Next free slot: %d\n",next_slot);
        printf("Page current data: %s\n", page_data); // Add +next_slot to get last slot
        printf("add_fixed_len_page - Added record successfully\n");
        printf("---------------------------\n");
    }

    return next_slot;
}

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page **page, int slot, Record *r){
    
    //Create buf and allocate enough space to handle the record
    void *buf = calloc(fixed_len_sizeof(r), sizeof(char));

    //Copy the record to the buffer
    int buffer_size = fixed_len_write(r, buf);

    //Find the position of a specific slot
    int next_slot = slot * (*page)->slot_size;

    //Copy buf content to the specific slot in the page
    char *data = (char*) (*page)->data;
    memcpy(data+next_slot, buf, strlen((char*)buf));

    if (DEBUG == TRUE){
        printf("write_fixed_len_page - Record updated in a specific slot successfully\n");
        printf("---------------------------\n");
    }
}

/**
 * Deserializes 'size' bytes from the buffer, 'buf', and
 * stores the record in 'record'.
 */
void fixed_len_read(void *buf, int size, Record *record) {

    // Requests that the vector capacity be at least enough to contain n elements
    record->reserve(ATTR_TOTAL);

    //Check how many attributs we have in this buf
    int attributs_quantity = size / REG_SIZE;

    //Values initialization
    for(int i = 0; i < attributs_quantity; i++){
        V *empty_array = new V[REG_SIZE];
        *empty_array = 0;
        record->push_back(*empty_array); 
    }
    
    std::vector<V>::iterator it;
    it = record->begin();
    
    char* temporary_array;
    for(int i = 0 ; i < attributs_quantity; i++) {
        temporary_array = new char[size];
        memcpy(temporary_array, (char *) buf + (REG_SIZE * i), REG_SIZE);    
        record->at(i) = (V) temporary_array;
    }
}

/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page **page, int slot, Record *r){

    char *buf = (char*) calloc(fixed_len_sizeof(r), sizeof(char));

    int specific_slot = slot * (*page)->slot_size;

    char *data = (char*) (*page)->data;
    memcpy(buf, data+specific_slot, (*page)->slot_size);

    fixed_len_read(buf, strlen(buf), r);

    free(buf);

    if (DEBUG == TRUE){
        printf("read_fixed_len_page - Read from an specific slot successfully\n");
        printf("---------------------------\n");
    }
}
/**
  * Return the number of pages in a Heapfile
  */
int number_of_pages(Heapfile **heapfile){
    if (DEBUG == TRUE){
        printf("number_of_pages - Returned the number of pages in the Heapfile successfully\n");
        printf("---------------------------\n");
    }
    // Example: (40B X 4B) / (2 * 4B) = 20 pages
    return ((*heapfile)->page_size * sizeof(int)) / (2 * sizeof(int));
}

/**
 * Return last page in the Heapfile
 * This function seek the file pointer to the last Heapfile page and return 
 * the heap_page_id of the last non 0
 */
int last_heapfile_page(Heapfile **heapfile){
    
    FILE *file = (*heapfile)->file_ptr;
    rewind(file);

    int heap_page_id = 0, last_heapfile_page_id = 0;
    int num_of_pages = number_of_pages(&(*heapfile));

    //Check if the Heapfile has been already created
    fread(&last_heapfile_page_id, sizeof(int), 1, file); 
    if(last_heapfile_page_id == -1){
        if(DEBUG == TRUE){
            printf("last_heapfile_page - Heapfile not created already\n");
            printf("---------------------------\n");
        }
        return -1;
    }
    
    do{  
        last_heapfile_page_id = heap_page_id;
        //page_size * offset * (pages_of_records + page of heap) 
        fseek(file, (*heapfile)->page_size * heap_page_id * (num_of_pages + 1), SEEK_SET);  
        fread(&heap_page_id, sizeof(int), 1, file); 
        if(DEBUG == TRUE){
            printf("last_heapfile_page - heap_page_id: %d\n", heap_page_id);
            printf("---------------------------\n");
        }
        
    }while(heap_page_id != 0);

    return last_heapfile_page_id;
}

/**
  *This is used to set free space to a new page in the end of the Heapfile
  */
void update_heapfile(Heapfile **heapfile){

    FILE *file = (*heapfile)->file_ptr;

    /**
     * Build the Heapfile
     * The first element is the link to other heap page
     * 0 means that is not such page
     */
    int num_of_pages = number_of_pages(&(*heapfile));
    int last_heapfile_page_id = last_heapfile_page(&(*heapfile));
    int offset = 0;
    int file_init = 0;

    //Update the link of the last Heapfile page;
    if((last_heapfile_page_id + 1) > 0){
        offset = last_heapfile_page_id * (num_of_pages+1) * (*heapfile)->page_size;
        fseek(file, offset, SEEK_SET);
        last_heapfile_page_id++;
        fwrite(&last_heapfile_page_id, sizeof(int), 1, file);
        last_heapfile_page_id--;
        if (DEBUG == TRUE){
            printf("init_heapfile|last_heapfile_page_id: %d, num_of_pages %d, offset %d, file_init %d, heapfile->page_size %d\n", 
                last_heapfile_page_id+1, num_of_pages, offset, file_init, (*heapfile)->page_size);
            printf("---------------------------\n");
        }
    }

    //Set the Offset to the new page
    offset = (last_heapfile_page_id+1) * (num_of_pages+1) * (*heapfile)->page_size;
    fseek(file, offset, SEEK_SET);
    //Set the pointer after the heap_page_id
    fwrite(&file_init, sizeof(int), 1, file);
    
    //Inicializates the page with blank values
    for(int i = 0; i < num_of_pages; i++){
        file_init = (last_heapfile_page_id+1) * (num_of_pages)+i;
        fwrite(&file_init, sizeof(int), 1, file);
        fwrite(&(*heapfile)->page_size, sizeof(int), 1, file);
    }

    if (DEBUG == TRUE){
        printf("update_heapfile - Initialized page with blank spaces successfully\n");
        printf("---------------------------\n");
    }
}

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile **heapfile, int page_size, FILE **file){

    (*heapfile)->page_size = page_size;
    (*heapfile)->file_ptr = *file;

    //Set the flag saying that the heap file dont exits yet;
    //Writes the int -1 to the beginning of the file.
    rewind(*file);
    int file_init = -1;
    fwrite(&file_init, sizeof(int), 1, *file);

    // Allocate space to a new page inside the brand new Heapfile
    update_heapfile(&(*heapfile));

    //Go to the begining of the file and write 0 to show that there is no other page
    rewind(*file);    
}

/**
  * Finds next slot within a Heapfile
  * If there is no slot returns -1;
  * or the heap_page_id if success
  */
int find_next_available_heapfile_slot(Heapfile **heapfile, int *_heap_page_id){
    FILE *file = (*heapfile)->file_ptr;
    int num_of_pages = number_of_pages(&(*heapfile));//# of pages slots inside a Heap Page
    int heap_page_id = 0, page_id = 0, free_space = 0;

    rewind(file);
    do{
        //This is the last heapPageId there is not 0
        *_heap_page_id = heap_page_id; 
                
        //Get the first heapfile_page_id
        fread(&heap_page_id, sizeof(int), 1, file); 
        //Go trough a Heap Page
        for(int j=0; j < num_of_pages; j++){
            fread(&page_id, sizeof(int), 1, file); 
            fread(&free_space, sizeof(int), 1, file); 
            if(free_space == (*heapfile)->page_size){
                if(DEBUG == TRUE){
                    printf("find_next_available_heapfile_slot - Found next slot within a Heapfile\n");
                    printf("---------------------------\n");
                }
                return page_id;
            }
                
        }
        //Go the next Heap Page
        fseek(file, (*heapfile)->page_size * heap_page_id * (num_of_pages+1), SEEK_SET);
    }while(heap_page_id != 0);
    
    if(DEBUG == TRUE){
        printf("find_next_available_heapfile_slot - No slot found in the Heapfile\n");
        printf("---------------------------\n");
    }
    return -1;
}

/**
  * Allocate another page in the heapfile. This grows the file by a page.
  */
PageID alloc_page(Heapfile **heapfile){

    FILE *file = (*heapfile)->file_ptr;

    //Fist we need to get the heap_page_id of the last page;
    int *next_heap_id =  new int();
    int next_slot = find_next_available_heapfile_slot(&(*heapfile), next_heap_id);

    //Check if there is any slot available on the Heapfile 
    if(next_slot != -1)
        return next_slot; // There isn't any free slot 
    
    // We need to create a new Heapfile page
    update_heapfile(&(*heapfile));

    // We should have a free slot now
    PageID page_id = find_next_available_heapfile_slot(&(*heapfile), next_heap_id);
    
    if(DEBUG == TRUE){
        printf("alloc_page - Allocated another page in the Heapfile successfully\n");
        printf("---------------------------\n");
    }
    return page_id;
}


/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile **heapfile, PageID pid){
    FILE *file = (*heapfile)->file_ptr;

    int offset, heap_page_id, slot_pid;
    int num_of_pages = number_of_pages(&(*heapfile));
    int _pid = pid;
    int free_space = fixed_len_page_freeslots(&page) * page->slot_size;
    
    // We need to write the pid in the slot and set the new space;
    // We need to go to the page pid offset
    // 1 - Find the heap_page_id of this pid
    heap_page_id = (( int ) pid) / num_of_pages;
    // 2 We need to find the slot of the pid within the HeapPageId
    slot_pid = (int) pid % num_of_pages;

    offset = (num_of_pages+1) * heap_page_id * (*heapfile)->page_size + (slot_pid) * (sizeof(int)*2) + sizeof(int);
    fseek(file,offset, SEEK_SET);
    fwrite(&_pid, sizeof(int), 1, file);
    fwrite(&free_space, sizeof(int), 1, file);

    // Find the right slot position to write the page
    int page_slot_offset = (((int) pid)/ num_of_pages) + ((int) pid) + 1;

    // GO to the offset position and write the data
    offset = page_slot_offset * (*heapfile)->page_size;
    fseek(file, offset, SEEK_SET);
    fwrite(page->data, sizeof(char), (*heapfile)->page_size, file);

    if(DEBUG == TRUE){
        printf("write_page - Page written to disk successfully\n");
        printf("---------------------------\n");
    }
}


/**
 * Read a page into memory
 */
void read_page(Heapfile **heapfile, PageID pid, Page *page){
    FILE *file = (*heapfile)->file_ptr;

    int offset, heap_page_id;
    int num_of_pages = number_of_pages(&(*heapfile));
    int page_pos; //Page position inside of a heap file page
    void *data = calloc(sizeof(int),(*heapfile)->page_size);
    
    // We need to go to the page pid offset
    // 1 - Find the heap_page_id of this pid
    int page_slot_offset = (((int) pid)/ num_of_pages) + ((int) pid) + 1;
    
    // 2 - Find the position of the page inside a Heap Page
    page_pos = (( int ) pid) % num_of_pages;
    offset =  page_slot_offset * (*heapfile)->page_size  + (page_pos * (*heapfile)->page_size);

    // Go to the offset position and retrieve the data
    fseek(file,offset,SEEK_SET);
    fread(data,sizeof(char),(*heapfile)->page_size,file);

    // Setting the data
    page->data = (void*) data;

    if(DEBUG == TRUE){
        printf("read_page - Page read into memory successfully\n");
        printf("---------------------------\n");
    }
}

int main(int argc, char **argv) {
    
    Page *page = new Page();
    Page *page2 = new Page();
    Record *record1 = new Record();
    Record *record2 = new Record();
    Record *record3 = new Record();
    Record *record4 = new Record();
    
    // First tuple
    record1->push_back((V) "5111222333\0");
    record1->push_back((V) "A111222333\0");
    // Second Tuple
    record2->push_back((V) "6111222333\0");
    record2->push_back((V) "B111222333\0");
    // Third Tuple
    record3->push_back((V) "7111222333\0");
    record3->push_back((V) "C111222333\0");
    // Forth Tuple
    record4->push_back((V) "8111222333\0");
    record4->push_back((V) "D111222333\0");
    
    // Initialize a page
    init_fixed_len_page(&page, 40, 20);

    // Add record to page
    signed int slot = 0; 
    slot = add_fixed_len_page(&page, record1);
    slot = add_fixed_len_page(&page, record2);
    
    // Fail to add record, page is full
    slot = add_fixed_len_page(&page, record3);

    // Initialize a page
    init_fixed_len_page(&page2, 40, 20);
    
    // Update the first slot content (change from record1 to record3)
    write_fixed_len_page(&page, 0, record3);    
    
    // Read from a specific page slot (reading from slot 1 to record4)
    read_fixed_len_page(&page, 1, record4);
    
    // Open file with write permission
    FILE *file = fopen("database.txt", "wb+");

    // Create and initialize Heapfile
    Heapfile *heapfile = new Heapfile();
    init_heapfile(&heapfile, 40, &file);

    // Adding 30 pages to the Heapfile
    int page_id = 0;
    for(int i = 0 ; i < 30; i++){    
        page_id = (int) alloc_page(&heapfile);
        write_page(page, &heapfile, page_id);
    }

    // Read a page into memory
    read_page(&heapfile, 0 ,page2);
    
    // Read a record from the page from a given slot.
    Record *new_record  = new Record();
    read_fixed_len_page(&page2, 0, new_record);  

    return 0;
}