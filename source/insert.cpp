#include "heapfile.h"



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
	int pid = alloc_page(heapfile);

	init_fixed_len_page(page,page_size,ATTR_TOTAL*REG_SIZE);
	page_nSlots = fixed_len_page_freeslots(page);
	read_page(heapfile,pid,page);
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
	
		printf("Insert |page_currentSlot %d  pid %d nSlots %d  page_size %ld, page_currentSlot %d\n",
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

int main(int argc, char const *argv[])
{
	
	if(argc != 4){
		printf("ARITY ERROR Found %d Wanted 3 args\n<heapfile> <csv_file> <page_size>",argc-1);
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
	fileCSV = fopen(argv[2],"r");
	fileHeapFile = fopen(argv[1],"rb+");
	
	//Create the heapfile with page_size argc[3]
	Heapfile * heapfile = new Heapfile;
	init_heapfile(heapfile,page_size,fileHeapFile);

	_insert(heapfile,fileCSV);
	return 0;
}