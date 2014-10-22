#include "heapfile.h"

int main(int argc, char const *argv[]) {

    if (argc != 4) {
        printf("ARITY ERROR Found %d Wanted 3 args\n<csv_file> <colstore_name> <page_size>",
                argc - 1);
        return -1;
    }

    // Iniciate the files
    FILE * fileCSV;
    // FILE * fileHeapFile[ATTR_TOTAL];
    // int size = 100;
    int attr_id = 0;
    int page_size = atoi(argv[3]);
    int page_nSlots[ATTR_TOTAL] = {0};
    int page_currentSlot[ATTR_TOTAL] = {0};
    int pid = -1;

    //set the colstore name directory
    char  *location_tmp = new char[100];
    char  *location_final = new char[100];
    strcat(location_tmp,"./");
    strcat(location_tmp,argv[2]);
    strcat(location_tmp,"/");
    char attr_name[100]={0};
    // Open the csv the file
    fileCSV = fopen(argv[1], "r");
    // Inicializate the records and pages	
    Record * record = new Record();
    // Page ** page = new Page*[ATTR_TOTAL];
    
    //Create the heapfiles with page_size argc[3]
    // Heapfile ** heapfile = new Heapfile*[ATTR_TOTAL];
    std::vector<Heapfile*> heapVec(ATTR_TOTAL);
    std::vector<Page*> pageVec(ATTR_TOTAL);
    std::vector<FILE *> fileHeapVec(ATTR_TOTAL);

    // Inicializate the heapfiles within the vector
    for (std::vector<Heapfile*>::iterator it = heapVec.begin(); it != heapVec.end(); ++it){
    	*it= new Heapfile();
    }

    // Inicializate the pages within the vector
    for (std::vector<Page*>::iterator it = pageVec.begin(); it != pageVec.end(); ++it){
    	*it= new Page();
    }
    // Inicializate the pages within the vector
    int _i =0 ;
    for (std::vector<FILE*>::iterator it = fileHeapVec.begin(); it != fileHeapVec.end(); ++it){
    	
    	strcpy(location_final,location_tmp);
   		snprintf(attr_name, sizeof(attr_name), "%d", _i);
   		strcat(location_final,attr_name);
        

        *it = fopen(location_final, "wb+");
        init_heapfile(heapVec.at(_i), page_size, *it);
        _i++;
    }



    for (int i = 0; i < ATTR_TOTAL; i++) {
 
        
        // init_heapfile(heapVec.at(i), page_size, fileHeapFile[i]);
    	init_fixed_len_page(pageVec.at(i), page_size,  REG_SIZE);
    	page_nSlots[i] = fixed_len_page_freeslots(pageVec.at(i));
    }

    //Auxilary variables of buffer and a temporary char to get ',' and '\n'
    char *buf = (char*) calloc(sizeof (char), REG_SIZE );
    char tmp;

    int csv_flag = 1;
    while (csv_flag) {
        //Reset the attr_id to zero
        if (attr_id == (ATTR_TOTAL))
            attr_id = 0;

        //Read a record from the csv file into buf
        // for (int i = 0; true; i += 10) {
            fread(buf, sizeof (char), 10, fileCSV);
            fread(&tmp, sizeof (char), 1, fileCSV);
            //change record
            // if (tmp == '\n') {
                // break;
            // }
        // }
        //If the file end store the remaining records
        if (tmp == EOF || feof(fileCSV)) {
            csv_flag = 0;
            //Write down all the pages that were not full yet
            for (int i = 0; i < ATTR_TOTAL; i++) {
                pid = alloc_page(heapVec.at(i));
                write_page(pageVec.at(i), heapVec.at(i), pid);
                //Clean the page
                printf("Page-data %s attr_id %d\n", pageVec.at(i)->data,attr_id);
                init_fixed_len_page(pageVec.at(i), page_size, REG_SIZE);
            }
            break;
        }

        // //Add the record in the rigth page	
        // pid = alloc_page(heapfile[attr_id]);
        // write_page(page[attr_id],heapfile, pid);
        // init_fixed_len_page(page[attr_id],page_size,REG_SIZE);


        // Store the buf in the record
        fixed_len_read(buf, REG_SIZE, record);
        //Write the reccord in the attr_id page 
        printf("buf %s it %d  pid %d nSlots %d  page_size %ld, page_currentSlot %d\n",
                buf ,attr_id, pid, page_nSlots[attr_id], strlen((char*) pageVec.at(attr_id)->data), page_currentSlot[attr_id]);
        page_currentSlot[attr_id] = add_fixed_len_page(pageVec.at(attr_id), record);

        // the number of free slots in this page
        page_nSlots[attr_id] = fixed_len_page_freeslots(pageVec.at(attr_id));


        //Write the page into heapfile when page full
        if (page_nSlots[attr_id] <= 0 || page_currentSlot[attr_id] == -1) {
            page_currentSlot[attr_id] = 0;
            pid = alloc_page(heapVec.at(attr_id));
            write_page(pageVec.at(attr_id), heapVec.at(attr_id), pid);
            //Clear the page
            init_fixed_len_page(pageVec.at(attr_id), page_size, REG_SIZE);
        }

        attr_id++;
    }

    
    free(location_tmp);
    free(location_final);
    free(buf);
    return 0;
}