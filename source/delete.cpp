#include "heapfile.h"

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

int main(int argc, char const *argv[])
{
		

	if(argc != 4){
		printf("ARITY ERROR Found %d Wanted 3 args\n<heapfile> <record_id> <page_size>"
			,argc-1);
		return -1;
	}


	// Iniciate the files
	FILE * fileHeapFile;
	FILE * fileRecordId;

	int size = 100;
	int page_size = atoi(argv[3]);
	int page_nSlots=0;
	int page_currentSlot =0;
	int pid = -1;

	// Iniciate the files
	fileHeapFile = fopen(argv[1],"rb+");
	fileRecordId = fopen(argv[2],"r");
	
	//Create the heapfile with page_size argc[3]
	Heapfile * heapfile = new Heapfile;
	init_heapfile(heapfile,page_size,fileHeapFile);

	// Inicializate the record and page	
	Record * record = new Record();	
	Page * page = new Page();

	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
	page_nSlots = fixed_len_page_freeslots(page);

	RecordID rid;
	rid.page_id = 0;
	rid.slot= 0;
	char tmp[50];
	fscanf (fileRecordId, "%s", tmp);
	rid.page_id = atoi(tmp);
	fscanf (fileRecordId, "%s", tmp);
	rid.slot = atoi(tmp);

	printf("Delete| rid.page_id %d rid.slot %d page_size %d\n",
		rid.page_id,rid.slot,page_size);

	//Call the update function core
	_delete(heapfile,rid);

	return 0;
}