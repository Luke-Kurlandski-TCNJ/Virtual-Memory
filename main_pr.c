/*
   Jack Taylor and Luke Kurlandski
   Dr. Yoon
   CSC345: Operating Systems
   Project 3: Virtual Memory
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/queue.h>
#include <stdbool.h>

#define NUM_FRAMES 128
#define WORD_SIZE 256
#define TLB_SIZE 16

// Analytical variables.
int total_refs = 0;
int page_faults = 0;
int TLB_hits = 0;
int tlb_counter = 0;

// [page number][0:frame number, 1:page offset, 2:valid/invalid bit]
int PAGE_TABLE[WORD_SIZE][3];
// [nothing][0: page number, 1: frame number]
int TLB[TLB_SIZE][2];
// Physical Memory.
int MEMORY[WORD_SIZE*NUM_FRAMES];
// Perform FIFO replacement in TLB.
int fr_count = 0;

// Queue storing page numbers for FIFO page replacement.
int intArray[NUM_FRAMES];
int front = 0;
int back = -1;
int totalItems = 0;

// Check if queue is full.
bool isFull() { return totalItems == NUM_FRAMES; }

// Return number of elements in queue.
int size() { return totalItems; }

// Insert a page to the end of the queue.
void insert(int data) {
	if(!isFull()) {
		if(back == NUM_FRAMES-1) {
			back = -1;
		}
		intArray[++back] = data;
		totalItems++;
   }
}

// Delete the oldest page in the queue.
int delete() {
	int data = intArray[front++];
	if(front == NUM_FRAMES) { 
		front = 0;
	}
	totalItems--;
	return data;  
}

// Calculate the page number from a logical address.
int get_page_number(int address) { return (255 & address>>8); }

// Calculate the offset for a logical address.
int get_page_offset(int address) { return (255 & address); }

// Return the frame number if the page number is in the TLB.
int search_TLB(int page_number) {
	for (int i=0; i<16; i++) {
		if (TLB[i][0] == page_number)
			return TLB[i][1];
	}
	return -1;
}

// Return the frame number if the page number is in page table.
int search_table(int page_number) {
	if (PAGE_TABLE[page_number][2] == 1)  
		return PAGE_TABLE[page_number][0];
	else
		return -1;
}

// Update the TLB.
void update_TLB(int page_number, int frame_number) {
	// Search TLB for an empty location.
	int emptySlot = 0;
	for (int i = 0; i < TLB_SIZE; i++) {
		if (TLB[i][0] == -1) {
			TLB[i][0] = page_number;
			TLB[i][1] = frame_number; 
			emptySlot = 1;
			break;
		}
	}
	// Remove element from TLB using FIFO.
	if (emptySlot == 0) {
		int newSlot = tlb_counter % TLB_SIZE;
		TLB[newSlot][0] = page_number;
		TLB[newSlot][1] = frame_number;
		tlb_counter++;
	}
}

// Return the frame number if a page fault occurs.
int page_fault(int page_number, int offset) {
	FILE* file;
	// This should be the first index of our desired page
	int beginIndex = WORD_SIZE * page_number;
	int maxChar = beginIndex + WORD_SIZE; 
	char str[maxChar];

	// read in the data from BACKING_STORE.bin	
	file = fopen("BACKING_STORE.bin", "rb");
	if (file == NULL) {
		printf("Could not open BACKING_STORE.bin\n");
		return -1;
	}
	fread(str, 1, maxChar, file);
	
	// Determine the frame number, next empty frame. 
	int frame_number = size();
	if (frame_number >= NUM_FRAMES) {
		// Get the frame_number to be deleted.
		int replace_page = delete();
		// Alter valid/invalid bit.
		PAGE_TABLE[replace_page][2] = 0;
		frame_number = PAGE_TABLE[replace_page][0];
		// Remove that page from the TLB
		for (int i = 0; i < TLB_SIZE; i++) {
			if (TLB[i][0] == replace_page) {
				TLB[i][0] = -1;
				TLB[i][1] = -1;
			}
		}
	}

	// Add page to queue.
	insert(page_number);

	// Fill the memory in the correct frame with contents.
	for (int i = 0; i < 256; i++) {
		MEMORY[frame_number*WORD_SIZE + i] = str[beginIndex + i];
	}

	// Close the file.
	fclose(file);
		
	// Update the TLB.
	update_TLB(page_number, frame_number);
	
	// Update page table
	PAGE_TABLE[page_number][0] = frame_number;
	PAGE_TABLE[page_number][1] = offset;
	PAGE_TABLE[page_number][2] = 1;
	
	return frame_number;
}

void address_parsing(char *f) {
	// File containing all logical addresses and output files.
	FILE *file = fopen(f, "r");
	FILE *f1 = fopen("out1.txt", "w");
	FILE *f2 = fopen("out2.txt", "w");
	FILE *f3 = fopen("out3.txt", "w");
	// Return if failures occur.
	if (file==NULL || f1==NULL || f2==NULL || f3==NULL) {
		printf("Could not open a file.\n");
		return;
	}

	int maxChar = 10;
	char str[maxChar];
	// Parse file, get info from each logical address, write
	while (fgets(str, maxChar, file) != NULL) {
		total_refs++;
		// Get info from the file.
		int logical_address = atoi(str);
		int page_number = get_page_number(logical_address);
		int offset = get_page_offset(logical_address);
		// Search the TLB for the page.
		int frame_number = search_TLB(page_number);
		if (frame_number < 0) {
			// Search page table for page if not found in TLB.
			frame_number = search_table(page_number);
			if (frame_number < 0) { 
				page_faults++;
				frame_number = page_fault(page_number, offset);
			}
		}
		else {
			TLB_hits++;
		}
		// Write info to the output files.
		fprintf(f1, "%d\n", logical_address);
		fprintf(f2, "%d\n", frame_number*WORD_SIZE + offset);
		fprintf(f3, "%d\n", MEMORY[frame_number*WORD_SIZE + offset]);
	}
	// Close all files.
	fclose(file);
	fclose(f1);
	fclose(f2);
	fclose(f3);
	return;
}

int main(int *argc, char **argv) {	

	// Initialize all TLB entries with -1 
	for (int i=0; i<16; i++) {
		for (int j=0; j<2; j++) 
			TLB[i][j] = -1;
	}
	
	// Scan the addresses.txt file.
	address_parsing(argv[1]);
	
	// Compute and print data analysis.
	float page_fault_rate = (float)page_faults / total_refs;
	float TLB_hit_rate = (float)TLB_hits / total_refs;
	printf("Page Fault Rate: %d/%d: %f\n", page_faults, total_refs, page_fault_rate);
	printf("TLB Hit Rate: %d/%d: %f\n", TLB_hits, total_refs, TLB_hit_rate);
	
	return 0;
}
