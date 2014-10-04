#include "heapfile.h"

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
	
	//Write the page in the heap again
	write_page(page,heapfile,pid);
	delete(page);
	delete(record);
}

int main(int argc, char const *argv[])
{
		

	if(argc != 6){
		printf("ARITY ERROR Found %d Wanted 5 args\n<heapfile> <record_id> <attribute_id> <new_value> <page_size>"
			,argc-1);
		return -1;
	}


	// Iniciate the files
	FILE * fileHeapFile;
	int size = 100;
	int page_size = atoi(argv[5]);
	int page_nSlots=0;
	int page_currentSlot =0;
	int pid = -1;

	// Iniciate the files
	fileHeapFile = fopen(argv[1],"wb+");
	
	//Create the heapfile with page_size argc[3]
	Heapfile * heapfile = new Heapfile;
	init_heapfile(heapfile,page_size,fileHeapFile);

	// Inicializate the record and page	
	Record * record = new Record();	
	Page * page = new Page();

	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
	page_nSlots = fixed_len_page_freeslots(page);

	int attr_id = atoi(argv[3]);
	
		//TODO CHECK THE RECORD ID 

	RecordID rid;
	rid.page_id = 0;
	rid.slot= 0;
	char * new_value = new char[strlen(argv[4])];
	new_value = (char*) argv[4];

	//Call the update function core
	_update(heapfile,rid,attr_id,new_value);

	return 0;
}