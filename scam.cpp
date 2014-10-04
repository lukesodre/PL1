#include "heapfile.h"

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

int main(int argc, char const *argv[])
{	

	if(argc != 3){
		printf("ARITY ERROR Found %d Wanted 2 args\n <heapfile> <page_size>",argc-1);
		return -1;
	}




	// Iniciate the files
	FILE * fileCSV;
	FILE * fileHeapFile;
	int size = 100;
	int page_size = atoi(argv[2]);
	int page_nSlots=0;
	int page_currentSlot =0;
	int pid = -1;



	// Iniciate the files
	fileHeapFile = fopen(argv[1],"rb+");
	
	//Create the heapfile with page_size argc[3]
	Heapfile * heapfile = new Heapfile;
	init_heapfile(heapfile,page_size,fileHeapFile);

	// Inicializate the record and page	
	Record * record = new Record();	
	Page * page = new Page();

	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
	page_nSlots = fixed_len_page_freeslots(page);


	_scan(heapfile);
	free(page);
	free(record);
	return 0;
}