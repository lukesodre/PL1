#include "heapfile.h"

void _select3(Heapfile * heapfile,Heapfile * heapfile_return, char* start, char* end) {
    FILE * file = heapfile->file_ptr;
    FILE * file_return = heapfile_return->file_ptr;
    rewind(file);
    rewind(file_return);
    //Create the Page and record 
    Page *page = new Page();
    Record *record = new Record();
    init_fixed_len_page(page, heapfile->page_size, ATTR_TOTAL * REG_SIZE);

    //Create the Page_return and record 
    Page *page_return = new Page();
    Record *record_return = new Record();
    init_fixed_len_page(page_return, heapfile_return->page_size, ATTR_TOTAL * REG_SIZE);

    //The number of Heap Pages and number of slots in each page;
    int heap_nPages = _HeapLastPage(heapfile) + 1;
    int heap_PageSlot = _nPages(heapfile);

    //The number of Heap Pages_return and number of slots in each page;
    int heap_nPages_return = _HeapLastPage(heapfile_return) + 1;
    int heap_PageSlot_return = _nPages(heapfile_return);

    int page_nSlots = fixed_len_page_capacity(page);
    int page_current = 0, rec_current = 0;
    RecordIterator *ite = new RecordIterator(heapfile);
    
    int page_nSlots_return = fixed_len_page_capacity(page_return);
    int page_current_return = 0, rec_current_return = 0;
    RecordIterator *ite_return = new RecordIterator(heapfile_return);

    char tmp[10]={0};

    while (ite->hasNext()) {
        *record = ite->next();

        if (rec_current == page_nSlots) {
            rec_current = 0;
            page_current++;
        }
        int attr = 0;
        for (std::vector<V>::iterator it = record->begin(); it != record->end(); ++it) {
            // printf("Page %d|Record %d |Attr %d |%s\n", page_current, rec_current, attr, *it);
            	// printf("record->at(0?)[0] %d >= start[0] %d record->at(0)[9] %d end[0] %d\n",
            		// record->at(0)[0], start[0] , record->at(0)[9],end[0]);
            
            while (ite_return->hasNext()) {
                *record_return = ite_return->next();

                if (rec_current_return == page_nSlots_return) {
                    rec_current_return = 0;
                    page_current_return++;
                }
                int attr_return = 0;
                for (std::vector<V>::iterator it_return = record_return->begin(); it_return != record_return->end(); ++it_return) { 
                
                    // select 
                    strcpy(tmp,*it_return);
                    if (tmp[0] >= start[0] && tmp[9] <= end[0]) {
                        printf("Select3 | %c%c%c%c%c\n",
                                tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);
                    }
                    if (*it_return == NULL)
                        break;
                
                    rec_current_return++;   
                }
            }
            if (*it == NULL)
                break;
                    
            rec_current++;
        }
    }
}

int main(int argc, char const *argv[]){

    if (argc != 7) {
        printf("ARITY ERROR Found %d Wanted 6 args\n<colstore_name> <attribute_id> <return_attribute_id> <start> <end> <page_size>",
                argc - 1);
        return -1;
    }

    // Iniciate the files
    FILE * fileHeapFile;
    FILE * fileHeapFile_return;
    // int size = 100;
    int attr_id = 0;
    int page_size = atoi(argv[6]);
    int page_nSlots[ATTR_TOTAL] = {0};
    int page_currentSlot[ATTR_TOTAL] = {0};
    int pid = -1;

    //set the colstore name directory
    char  *location_tmp = new char[100];
    char  *location_final = new char[100];
    strcat(location_tmp,"./");
    strcat(location_tmp,argv[1]);
    strcat(location_tmp,"/");
    // char attr_name[100]={0};
    
    strcpy(location_final,location_tmp);
    // set the return_attribute_id name directory
    char *location_return_attr = new char[100];
    strcpy(location_return_attr, location_final);
    
    strcat(location_final,(char*)argv[2]);
    strcat(location_return_attr,(char*)argv[3]);

    printf("%s -- %s\n", location_final, location_return_attr);
    
    Heapfile * heapfile;
    fileHeapFile = fopen(location_final, "rb+");
    init_heapfile(heapfile, page_size, fileHeapFile);

    // heapfile to hold return_attribute_id pages
    Heapfile * heapfile_return;
    fileHeapFile_return = fopen(location_return_attr, "rb+");
    init_heapfile(heapfile_return, page_size, fileHeapFile_return);

    _select3(heapfile,heapfile_return,(char*)argv[4],(char*)argv[5]);
    
    free(location_tmp);
    free(location_final);

    free(location_return_attr);
    return 0;
}