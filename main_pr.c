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

#define MAX 128

// Analytical variables.
int total_refs = 0;
int page_faults = 0;
int TLB_hits = 0;

// index:page number
// value:(0:frame number, 1:page offset, 2:valid/invalid bit)
int PAGE_TABLE[256][3];
// index:nothing, value:(0:page number, 1:frame number)
int TLB[16][2];
// physical memory
int MEMORY[256*MAX];
int fr_count = 0;

// Queue for implementing FIFO page replacement.
int intArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

bool isFull() { return itemCount == MAX; }
int size() { return itemCount; }  

void insert(int data) {
   if(!isFull()) {
      if(rear == MAX-1)
         rear = -1;            
      intArray[++rear] = data;
      itemCount++;
   }
}

int delete() {
   int data = intArray[front++];
   if(front == MAX) 
      front = 0;
   itemCount--;
   return data;  
}

// Calculate the page number from a logical address.
int page_number(int address) { return (255 & address>>8); }

// Calculate the offset for a logical address.
int page_offset(int address) { return (255 & address); }

// Return the frame number if the page number is in the TLB.
int search_TLB(int number) {
	for (int i=0; i<16; i++) {
		if (TLB[i][0] == number)
			return TLB[i][1];
	}
	return -1;
}

// Return the frame number if the page number is in page table.
int search_table(int number) {
	if (PAGE_TABLE[number][2] == 1)  
		return PAGE_TABLE[number][0];
	else // Page Fault 
		return -1;
}

// Return the frame number if a page fault occurs.
int pageFault(int pageNumber, int offset) {
	FILE* file;
	// This should be the first index of our desired page
	int beginIndex = 256 * pageNumber;
	int maxChar = beginIndex + 256; 
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
	int pageNo = 0;
	if (frame_number >= 128) {
		// Get the frame_number to be deleted.
		frame_number = delete();
		// Get the page with that frame_number.
		for (int i=0; i<256; i++) {
			if (PAGE_TABLE[i][0] == frame_number) {
				pageNo = i;
				break;
			}
		}
		// Alter valid/invalid bit.
		PAGE_TABLE[pageNo][2] = 0;

		// Remove that page from the TLB
		for (int i = 0; i < 16; i++) {
			if (TLB[i][0] == pageNo) {
				TLB[i][0] = -1;
				TLB[i][1] = -1;
			}
		}
	}
	// Add item to queue.
	insert(frame_number);

	// Fill the memory in the correct frame.
	for (int i = 0; i < 256; i++) {
		MEMORY[frame_number*256 + i] = str[beginIndex + i];
	}
	fclose(file);
	
	// Update the TLB
	int emptySlot = 0;
	for (int i = 0; i < 16; i++) {
		if (TLB[i][0] == -1) {
			TLB[i][0] = pageNumber;
			TLB[i][1] = frame_number; 
			emptySlot = 1;
			break;
		}
	}
	if (emptySlot == 0) {
		int newSlot = rand() % 16;
		TLB[newSlot][0] = pageNumber;
		TLB[newSlot][1] = frame_number; 
	}
	
	// Update page table
	PAGE_TABLE[pageNumber][0] = frame_number;
	PAGE_TABLE[pageNumber][1] = offset;
	PAGE_TABLE[pageNumber][2] = 1;
	
	return frame_number;
}

void addressParsing(char *f) {
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
		int address = atoi(str);
		int number = page_number(address);
		int offset = page_offset(address);
		//printf("Number: %d Offset: %d\n", number, offset);
		
		// Search the TLB for the page.
		int frame_number = search_TLB(number);
		if (frame_number < 0) {
			// Search page table for page.
			frame_number = search_table(number);
			if (frame_number < 0) { 
				page_faults++;
				frame_number = pageFault(number, offset);
			}
		}
		else {
			TLB_hits++;
		}
		// Write info to the output files.
		fprintf(f1, "%d\n", address);
		fprintf(f2, "%d\n", frame_number*256 + offset); //FIXME
		fprintf(f3, "%d\n", MEMORY[frame_number*256+offset]); //FIXME
	}
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
	
	addressParsing(argv[1]);
	
	float page_fault_rate = (float)page_faults / total_refs;
	float TLB_hit_rate = (float)TLB_hits / total_refs;

	printf("Page Fault Rate: %d/%d: %f\n", page_faults, total_refs, page_fault_rate);
	printf("TLB Hit Rate: %d/%d: %f\n", TLB_hits, total_refs, TLB_hit_rate);
	return 0;
}
