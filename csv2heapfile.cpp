// #include "csv2heapfile.h"
#include "heapfile.h"


void _select(Heapfile * heapfile,int attr_id, int start, int end){
	FILE * file = heapfile->file_ptr;
	rewind(file);
	
	// Inicializate the record and page	
	Record * record = new Record();	
	Page * page = new Page();
	int page_size = heapfile->page_size;
	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);

	//The number of Heap Pages and number of slots in each page;
	int heap_nPages = _HeapLastPage(heapfile) +1;
	int heap_PageSlot = _nPages(heapfile);

	int page_nSlots = fixed_len_page_capacity(page);
	int page_current =0,rec_current=0;
	RecordIterator *ite = new RecordIterator(heapfile);


	while(ite->hasNext()){
		rec_current++;
		*record = ite->next();
		
		if(rec_current == page_nSlots){
			rec_current =0;
			page_current++;
		}
		// for(int attr = 0; attr < ATTR_TOTAL ;attr++)
			
			printf("Page %d|Record %d |Attr %d |%s\n", page_current,rec_current,attr_id,record->at(attr_id)); 

	}
}


void _insert(Heapfile *heapfile, FILE * fileCSV){
	FILE * file = heapfile->file_ptr;

	//Rewind the files
	rewind(file);
	rewind(fileCSV);
	// Inicializate the record and page	
	Record * record = new Record();	
	Page * page = new Page();

	int size = 100;
	int page_size = heapfile->page_size;
	int page_nSlots=0;
	int page_currentSlot =0;
	int pid = -1;

	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
	page_nSlots = fixed_len_page_freeslots(page);

	//Inicializating temporary buffer and tmp char used to grad ',' and '\n' 
	char *buf = (char*) calloc(sizeof(char),size*ATTR_TOTAL);
	char tmp;
	
	//Flag that sinalize the end of the file
	int csv_flag = 1;
	while(csv_flag){
		//Read a record from the csv file into buf
		for(int i =0 ; true ;i+=10){
			fread(buf+i,sizeof(char),10,fileCSV);
			fread(&tmp,sizeof(char),1,fileCSV);
			//change record
			if(tmp == '\n'){
				break;
			}
		}
		//If the file end store the remaining records
		if(tmp == EOF || feof(fileCSV)){
			csv_flag = 0;
			pid = alloc_page(heapfile);
			write_page(page,heapfile, pid);
			//Clean the page
			init_fixed_len_page(page,page_size,REG_SIZE);
			break;
		}
			
		// Store the buf in the record
		fixed_len_read(buf,REG_SIZE*ATTR_TOTAL,record);
		page_currentSlot =  add_fixed_len_page(page,record);
		// write_fixed_len_page(page,page_currentSlot,record);
		
		//Check the number of free slots in this page
		page_nSlots = fixed_len_page_freeslots(page);
	
		printf("buf  it %d  pid %d nSlots %d  page_size %ld, page_currentSlot %d\n",
			page_currentSlot, pid,page_nSlots,strlen((char*)page->data),page_currentSlot);
	
		//Write the page into heapfile when page full
		if(page_nSlots <= 0 || page_currentSlot == -1){
			page_currentSlot = 0;		
			pid = alloc_page(heapfile);
			write_page(page,heapfile, pid);
			//Clear the page
			init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
		}
		// page_currentSlot++;
	}


}

void _delete(Heapfile* heapfile,RecordID rid){
	FILE * file = heapfile->file_ptr;
	
	PageID pid = (PageID) rid.page_id;

	//Create the Page and record 
	Page *page = new Page();
	Record *record = new Record();
	init_fixed_len_page(page,heapfile->page_size,ATTR_TOTAL*REG_SIZE);

	//Get the page where the record is
	read_page(heapfile,pid,page);
	//Get the record
	read_fixed_len_page(page,rid.slot,record);
	char * tmp = (char*)calloc(sizeof(char),REG_SIZE);


	//Delete the record
	for(int attr_id = 0 ; attr_id < ATTR_TOTAL ; attr_id++)
		record->at(attr_id) = "0000000000";
	
	printf("delete| record %s\n",(char*) record->at(0) );
	//Write the record in the page
	write_fixed_len_page(page,rid.slot,record);
	
	//Write the page in the heap again
	write_page(page,heapfile,pid);
	
     // update the heap page 
	int freeSpace = 0;
	int nPages = _nPages(heapfile);
	int pageHeapId = (((int) pid)/ nPages);
	int slot = ((int) pid) % nPages;
	int offset =  pageHeapId* heapfile->page_size + (sizeof(int)*2 + (sizeof(int)*2)*slot);
	fseek(file,offset,SEEK_SET);
	//Read the freespace amount
    fread(&freeSpace,sizeof(int),1,file);
    freeSpace -= REG_SIZE*ATTR_TOTAL;
    //Update the new freespace
	fseek(file,offset,SEEK_SET);
    fwrite(&freeSpace,sizeof(int),1,file);

	delete(page);
	delete(record);


}

void _update(Heapfile* heapfile,RecordID rid, int attr_id,void * new_value){
	FILE * file = heapfile->file_ptr;
	
	PageID pid = (PageID) rid.page_id;


	//Create the Page and record 
	Page *page = new Page();
	Record *record = new Record();
	init_fixed_len_page(page,heapfile->page_size,ATTR_TOTAL*REG_SIZE);

	//Get the page where the record is
	read_page(heapfile,pid,page);
	//Get the record
	read_fixed_len_page(page,rid.slot,record);

	//Update the new attribute
	// printf("update| Page %s\n",(char*) page->data );
	record->at(attr_id) = (char*) new_value;
	
	//Write the record in the page
	write_fixed_len_page(page,rid.slot,record);
	
	puts("agora");
	//Write the page in the heap again
	write_page(page,heapfile,pid);
	delete(page);
	delete(record);
}

void _scan(Heapfile *heapfile){
	FILE * file = heapfile->file_ptr;
	rewind(file);
	//Create the Page and record 
	Page *page = new Page();
	Record *record = new Record();
	init_fixed_len_page(page,heapfile->page_size,ATTR_TOTAL*REG_SIZE);

	//The number of Heap Pages and number of slots in each page;
	int heap_nPages = _HeapLastPage(heapfile) +1;
	int heap_PageSlot = _nPages(heapfile);

	int page_nSlots = fixed_len_page_capacity(page);
	int page_current =0,rec_current=0;
	RecordIterator *ite = new RecordIterator(heapfile);


	while(ite->hasNext()){
		rec_current++;
		*record = ite->next();
		
		if(rec_current == page_nSlots){
			rec_current =0;
			page_current++;
		}
		for(int attr = 0; attr < ATTR_TOTAL ;attr++)
			printf("Page %d|Record %d |Attr %d |%s\n", page_current,rec_current,attr,record->at(attr)); 

	}
	
}



int  main(int argc, char const *argv[])
{	

	if(argc != 4){
		printf("ARITY ERROR Found %d Wanted 3 args\n",argc-1);
		return -1;
	}

	// Iniciate the files
	FILE * fileCSV;
	FILE * fileHeapFile;
	int size = 100;
	int page_size = atoi(argv[3]);
	int page_nSlots=0;
	int page_currentSlot =0;
	int pid = -1;

	// Iniciate the files
	fileCSV = fopen(argv[1],"r");
	fileHeapFile = fopen(argv[2],"wb+");
	
	//Create the heapfile with page_size argc[3]
	Heapfile * heapfile = new Heapfile;
	init_heapfile(heapfile,page_size,fileHeapFile);

	// Inicializate the record and page	
	Record * record = new Record();	
	Page * page = new Page();

	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
	page_nSlots = fixed_len_page_freeslots(page);

	char *buf = (char*) calloc(sizeof(char),size*ATTR_TOTAL);
	// char *buf2 = (char*) calloc(sizeof(char),size);
	char tmp;
	
	int csv_flag = 1;
	while(csv_flag){
		//Read a record from the csv file into buf
		for(int i =0 ; true ;i+=10){
			fread(buf+i,sizeof(char),10,fileCSV);
			fread(&tmp,sizeof(char),1,fileCSV);
			//change record
			if(tmp == '\n'){
				break;
			}
		}
		//If the file end store the remaining records
		if(tmp == EOF || feof(fileCSV)){
			csv_flag = 0;
			pid = alloc_page(heapfile);
			write_page(page,heapfile, pid);
			//Clean the page
			init_fixed_len_page(page,page_size,REG_SIZE);
			break;
		}
			
		// Store the buf in the record
		fixed_len_read(buf,REG_SIZE*ATTR_TOTAL,record);
		page_currentSlot =  add_fixed_len_page(page,record);
		// write_fixed_len_page(page,page_currentSlot,record);
		
		//Check the number of free slots in this page
		page_nSlots = fixed_len_page_freeslots(page);
	
		printf("buf  it %d  pid %d nSlots %d  page_size %ld, page_currentSlot %d\n",
			page_currentSlot, pid,page_nSlots,strlen((char*)page->data),page_currentSlot);
	
		//Write the page into heapfile when page full
		if(page_nSlots <= 0 || page_currentSlot == -1){
			page_currentSlot = 0;		
			pid = alloc_page(heapfile);
			write_page(page,heapfile, pid);
			//Clear the page
			init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
		}
		// page_currentSlot++;
	}

	RecordID rid;
	rid.page_id = 0;
	rid.slot = 0;
	int attr_id = 4;
	char *new_value = "LUCASLUCAS"; 
	_update(heapfile,rid,attr_id,(void* )new_value);
	_delete(heapfile,rid);
	_insert(heapfile,fileCSV);
	_scan(heapfile);
	// fixed_len_write(record,buf2);

		// printf("%s\n",buf2 );
	return 0;
}
