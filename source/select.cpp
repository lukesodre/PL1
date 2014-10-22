#include "heapfile.h"
#include <iostream>
#include <sys/timeb.h>

int getMilliCount(){
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

int getMilliSpan(int nTimeStart){
    int nSpan = getMilliCount() - nTimeStart;
    if(nSpan < 0)
        nSpan += 0x100000 * 1000;
    return nSpan;
}

void _select(Heapfile * heapfile, int attr_id, char* start, char* end) {
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
    char tmp[10];

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
            if (attr == attr_id) {
                strcpy(tmp,*it);
                if (tmp[0] >= start[0] && tmp[9] <= end[0]) {
                    printf("Select | %c%c%c%c%c\n",
                            record->at(0)[1], record->at(0)[2], record->at(0)[3], record->at(0)[4], record->at(0)[5]);
                }

            attr++;
                if (attr == ATTR_TOTAL)
                    break;
            }
            rec_current++;

        }
    }
}


int main(int argc, char const *argv[]) {

    printf("Starting timer...\n");
    int start = getMilliCount();

    if (argc != 6) {
        printf("ARITY ERROR Found %d Wanted 5 args\n<heapfile> <attribute_id> <start> <end> <page_size>",
         argc - 1);
        return -1;
    }

    // Iniciate the files
    FILE * fileHeapFile;
    
    //Grab the parameters
    int attr_id = atoi(argv[2]);
    int page_size = atoi(argv[5]);

    //set the colstore name directory
    char  *location_tmp = new char[100];
    char  *location_final = new char[100];
    strcat(location_tmp,"./");
    strcat(location_tmp,argv[1]);
    strcat(location_tmp,"/");
    // char attr_name[100]={0};

    strcpy(location_final,location_tmp);
    strcat(location_final,(char*)argv[2]);

    //printf("%s\n", location_final);

    Heapfile * heapfile = new Heapfile();
    fileHeapFile = fopen(location_final, "rb+");
    init_heapfile(heapfile, page_size, fileHeapFile);

    //Call the core function  
    _select(heapfile,attr_id,(char*)argv[3],(char*)argv[4]);

    free(location_tmp);
    free(location_final);

    int milliSecondsElapsed = getMilliSpan(start);

    printf("Elapsed time = %u milliseconds\n", milliSecondsElapsed);

    return 0;
}