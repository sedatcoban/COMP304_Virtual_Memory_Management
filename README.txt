COMP 304 
Spring 2021
Project 3

Project Members:
Sedat Çoban 60545
Burcu Özer 64535

Part I
To run the code, after compiling, backing store file is taken as the first argument and the logical address file is taken as the second argument.
Inside the code, backing file and adrress file are opened. Page table is initialized by making all values equal to -1. The address file is read in a while loop. 
The page offset and logical page number were calculated from logical_address, offset_mask, offset_bits and page_mask.
offset_mask is 1023 because we want to get the first 10 bit from the logical_adress. offset_bits is 10 because we have to slide logical adress 10 bits 
to bring the logical_page number to beginning. page_mask is 1023 because we want to get the first 10 bit from the logical adress.
Then, logical page was searched in the TLB table with the return value of physical page. If pyhsical page is not equal to -1, we have a TLB hit.
Otherwise, TLB miss occurs. Logical page is searched in the page table. If the return value (physical_page) is equal to -1, it means a page fault happened.
Therefore, we have to find the logical page from the backup and load it into the main memory. Empty frame within the main memory was found by free_page%PAGES.
Content of the backup was copied to the allocated free page in the memory. Page table and TLB table are updated according to this newly allocated frame.
Logical address, physical adress and value are printed.
Total tlb_hit, tlb hit rate, page fault, page fault rate, and the number of translated adresses were printed.

Part II
To run the code, after compiling, backing store file is taken as the first argument and the logical address file is taken as the second argument. 
Third argument is -p and fourth argument is 0 or 1 to denote the FIFO and LRU replecement policy.
Since the virtual memory is larger than the main memory (1024 pages vs 256 frames) in this part, the memory size was defined as 256 * PAGE_SIZE at the beginining. 
Inside the code, backing file and adrress file are opened. Page table is initialized by making all values equal to -1. lru_table was initialized by making 
all logical and read values equal to -1. The address file is read in a while loop. 
The page offset and logical page number were calculated from logical_address, offset_mask, offset_bits and page_mask.
offset_mask is 1023 because we want to get the first 10 bit from the logical_adress. offset_bits is 10 because we have to slide logical adress 10 bits 
to bring the logical_page number to beginning. page_mask is 1023 because we want to get the first 10 bit from the logical adress.
Then, logical page was searched in the TLB table with the return value of physical page. If pyhsical page is not equal to -1, we have a TLB hit.
Otherwise, TLB miss occurs. Logical page is searched in the page table. If the return value (physical_page) is equal to -1, it means a page fault happened.
Therefore, we have to find the logical page from the backup and load it into the main memory.According to specification of page replacement policy, we have two different implementations. 

If p is equal to 0 (FIFO implementation):
Empty frame within the main memory was found by free_page%256.
Content of the backup was copied to the allocated free page in the memory.
The page table was updated to remove the entries assigned to the newly allocated physical page.
Page table and TLB table are updated according to this newly allocated frame.

If p is equal to 1 (LRU implementation):
We have to find the least recent used frame by iterating lru table. 
After finding the entry (consist of logical page number and read value to keep the track of the how recent pages are), 
we assigned the index of least recently used frame to physical page. 
We updated the least recently used entry of the lru_table with the new logical_page and total_addresses (new read value in the struct).
Content of the backup was copied to the allocated free page in the memory.
The page table was updated to remove the entries assigned to the newly allocated physical page.  
However, if the frame is in the page or tlb table, read value of the frame was updated to total_addresses since it is used most recently.
TLB table is updated according to this newly allocated frame.

At the end:
Logical address, physical adress and value are printed.
Total tlb_hit, tlb hit rate, page fault, page fault rate, and the number of translated adresses were printed.