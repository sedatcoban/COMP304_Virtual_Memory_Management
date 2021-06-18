/**
 * virtmem.c 
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 1024
#define PAGE_MASK 1023 // it is equals to 1111111111

#define PAGE_SIZE 1024
#define OFFSET_BITS 10 // slide 10 bits of the logical_address
#define OFFSET_MASK 1023 // it is equals to 1111111111

#define MEMORY_SIZE 256 * PAGE_SIZE
// Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

struct tlbentry {
  int logical;
  int physical;
};

//Data structure to keep least recently used frame
struct lru{
	int logical; //keeping logical_page of the frame 
	int read; //helps us to understand the least recently used frame
};
struct lru lruTable[256]; //Array which keeps least recently used frames

// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
struct tlbentry tlb[TLB_SIZE];
// number of inserts into TLB that have been completed. Use as tlbindex % TLB_SIZE for the index of the next TLB line to use.
int tlbindex = 0;

// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
int pagetable[PAGES];

signed char main_memory[MEMORY_SIZE];

// Pointer to memory mapped backing file
signed char *backing;

int max(int a, int b)
{
  if (a > b)
    return a;
  return b;
}

/* Returns the physical address from TLB or -1 if not present. */
int search_tlb(int logical_page) {
    /* TODO */
    for (int i = 0; i<TLB_SIZE; i++){ //Iterates the tlb_table to find the logical_page entry.
    	struct tlbentry *TLBentry = &tlb[i%TLB_SIZE];
    	int tlbLogical = TLBentry->logical;
    	if(logical_page==tlbLogical){
    		
    		return TLBentry->physical;
    	}
    	
    }

    return -1;
}

/* Adds the specified mapping to the TLB, replacing the oldest mapping*/
void add_to_tlb(int logical, int physical) {
    /* TODO */
    struct tlbentry *newEntry;
    if(tlbindex==TLB_SIZE-1){
    	tlbindex=0;
    }
    tlb[tlbindex%TLB_SIZE]=*newEntry; //Adding new entry to the tlb_table according to its new place
    tlbindex++;
    newEntry->logical = logical;
    newEntry ->physical = physical;
    
    
}

int main(int argc, const char *argv[])
{
// Taking arguments from command line
  if (argc != 5) {
    fprintf(stderr, "Usage ./virtmem backingstore input\n");
    exit(1);
  }
  int p=-1;
  if(strcmp("-p",argv[3])==0){
  	if(strcmp(argv[4],"1")==0){
  		p=1;
  	}else if(strcmp(argv[4],"0")==0){
  		p=0;
  	}else{
  		fprintf(stderr, "Wrong spesification of p\n");
  		exit(1);
  	}
  }else{
  	fprintf(stderr, "Specify p argument\n");
  	exit(1);
  }
  
  const char *backing_filename = argv[1]; 
  int backing_fd = open(backing_filename, O_RDONLY);
  backing = mmap(0, PAGES*PAGE_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 
  
  const char *input_filename = argv[2];
  FILE *input_fp = fopen(input_filename, "r");
  
  // Fill page table entries with -1 for initially empty table.
  int i;
  for (i = 0; i < PAGES; i++) {
    pagetable[i] = -1;
  }
  
  //Initializing lru table.
  for(int x = 0 ; x<256; x++){
 	struct lru *new = &lruTable[x];
        new->logical = -1;
        new->read = -1;
        lruTable[x]=*new;
  }
  
  // Character buffer for reading lines of input file.
  char buffer[BUFFER_SIZE];
  
  // Data we need to keep track of to compute stats at end.
  int total_addresses = 0;
  int tlb_hits = 0;
  int page_faults = 0;
  
  // Number of the next unallocated physical page in main memory
  int free_page = 0;
  
  while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL) {
    total_addresses++;
    int logical_address = atoi(buffer);

    /* TODO 
    / Calculate the page offset and logical page number from logical_address */
    int offset = logical_address & OFFSET_MASK; //AND is used to get the first 10 bit as offset
    int logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK; //After sliding the logical_address we used AND to get the first 10 bit as logical_page
    ///////
    
    int physical_page = search_tlb(logical_page); //searching logical_page within the tlb table
    
    // TLB hit
    if (physical_page != -1) {
      tlb_hits++;
  	
      // TLB miss
    } else {
      physical_page = pagetable[logical_page]; //searching logical_page within the page_table
      
      // Page fault
      if (physical_page == -1) {
          /* TODO */
          if(p==0){ //If FIFO is chosen
          page_faults++;
          physical_page = free_page%256; //Finds next free page
          free_page++;
          memcpy(main_memory + physical_page * PAGE_SIZE, backing + logical_page*PAGE_SIZE , PAGE_SIZE); //Copy the content of the backup to main memory
          int count = 0;
          while(count<PAGES){ //Updates the page table to remove the entries assigned to the newly allocated physical page
	 	if(pagetable[count] == physical_page){
	 		pagetable[count] = -1;
	 	}
	 	count++;
	  }
          pagetable[logical_page]=physical_page; //Adds the new physical page-logical page association to the page table
          }else if(p = 1){ //If LRU is chosen
          	page_faults++;
          	int index = 0; 
          	int minRead = 10000;
          	for(int m = 0 ; m<256; m++){ //finding the least recently used frame by iterating lru table.
          		struct lru *LRUentry = &lruTable[m];
          		int read = LRUentry->read;
          		if(read<minRead){
          			index = m;
          			minRead = read;
          		}
          	}
          	
          	physical_page = index; //assigning index of the least recently used frame to physical page.
          	struct lru *new = &lruTable[index];
          	new->logical = logical_page;
    		new->read = total_addresses; //total_addresses are used to know which frame is recently used.
          	lruTable[index]=*new;
    		
    		memcpy(main_memory + physical_page * PAGE_SIZE, backing + logical_page*PAGE_SIZE , PAGE_SIZE); //Copy the content of the backup to main memory
          	int loop = 0;
          	while(loop<PAGES){ //Updates the page table to remove the entries assigned to the newly allocated physical page
	 	if(pagetable[loop] == physical_page){
	 		pagetable[loop] = -1;
	 	}
	 	loop++;
	  	}
	  	pagetable[logical_page]=physical_page; //Adds the new physical page-logical page association to the page table
          }
       }
       	for(int k = 0 ; k<256; k++){ //If the frame is in the page or tlb table, read value of the frame was updated to total_addresses since it is used most recently. 
       	struct lru *LRUentry = &lruTable[k];
       	int log = LRUentry->logical;
       	if(log==logical_page){
       		LRUentry->read = total_addresses;
       	}
       }	
      add_to_tlb(logical_page, physical_page); //updating tlb table
    }
    
    int physical_address = (physical_page << OFFSET_BITS) | offset;
    signed char value = main_memory[physical_page * PAGE_SIZE + offset];
    
    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
  }
  printf("Number of Translated Addresses = %d\n", total_addresses);
  printf("Page Faults = %d\n", page_faults);
  printf("Page Fault Rate = %.3f\n", page_faults / (1. * total_addresses));
  printf("TLB Hits = %d\n", tlb_hits);
  printf("TLB Hit Rate = %.3f\n", tlb_hits / (1. * total_addresses));
  
  return 0;
}
