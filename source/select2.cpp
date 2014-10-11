#include "heapfile.h"

void _select2(Heapfile * heapfile, char* start, char* end) {
    FILE * file = heapfile->file_ptr;
    rewind(file);
    //Create the Page and record 
    Page *page = new Page();
    Record *record = new Record();
    init_fixed_len_page(page, heapfile->page_size, ATTR_TOTAL * REG_SIZE);

    //The number of Heap Pages and number of slots in each page;
    int heap_nPages = _HeapLastPage(heapfile) + 1;
    int heap_PageSlot = _nPages(heapfile);

    int page_nSlots = fixed_len_page_capacity(page);
    int page_current = 0, rec_current = 0;
    RecordIterator *ite = new RecordIterator(heapfile);
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
            
            strcpy(tmp,*it);
            if (tmp[0] >= start[0] && tmp[9] <= end[0]) {
                printf("Select2 | %c%c%c%c%c\n",
                        tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);
            }

            if (*it == NULL)
                break;
            
            rec_current++;

        }
    }
}

int main(int argc, char const *argv[]){

    if (argc != 6) {
        printf("ARITY ERROR Found %d Wanted 5 args\n<colstore_name> <attribute_id> <start> <end> <page_size>",
                argc - 1);
        return -1;
    }

    // Iniciate the files
    FILE * fileHeapFile;
    // int size = 100;
    int attr_id = 0;
    int page_size = atoi(argv[5]);
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
    strcat(location_final,(char*)argv[2]);
    printf("location %s", location_final);

    
    Heapfile * heapfile;
    fileHeapFile = fopen(location_final, "rb+");
    init_heapfile(heapfile, page_size, fileHeapFile);

    _select2(heapfile,(char*)argv[3],(char*)argv[4]);
    
    free(location_tmp);
    free(location_final);
    return 0;
}