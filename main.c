/*
   Jack Taylor and Luke Kurlandski
   Dr. Yoon
   CSC345: Operating Systems
   Project 3: Virtual Memory
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// index:page number
// value:(0:frame number, 1:page offset, 2:valid/invalid bit)
int PAGE_TABLE[256][3];
// index:nothing, value:(0:page number, 1:frame number)
int TLB[16][2];
// physical memory
int MEMORY[256*256];
// variables used to track TLB hits and page faults
int total_refs, tlb_hits, page_faults = 0;
// for case of no page replacement, this keeps track of the frame to be used
int current_frame = 0;
// for tlb replacement, keep count
int tlb_counter = 0;

// Calculate the page number from a logical address.
int page_number(int address) {
	// 255 = 000000001111111100000000
	return (255 & address>>8);
}

// Calculate the offset for a logical address.
int page_offset(int address) {
	// 255 = 000000000000000011111111
	return (255 & address);
}

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
	page_faults += 1;

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
	
	int frame_number = current_frame;

	for (int i = 0; i < 256; i++) {
		MEMORY[frame_number*256 + i] = str[beginIndex + i];
	}

	current_frame++;

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
		int newSlot = tlb_counter  % 16;
		TLB[newSlot][0] = pageNumber;
		TLB[newSlot][1] = frame_number;
		tlb_counter++;
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
		total_refs += 1;

		// Get info from the file.
		int address = atoi(str);
		int number = page_number(address);
		int offset = page_offset(address);
		
		// Search the TLB for the page.
		int frame_number = search_TLB(number);
		if (frame_number < 0) {
			// Search page table for page.
			frame_number = search_table(number);
			if (frame_number < 0) { 
				frame_number = pageFault(number, offset);
			}
		}
		else {
			tlb_hits += 1;
		}
		// Write info to the output files.
		fprintf(f1, "%d\n", address);
		fprintf(f2, "%d\n", frame_number*256 + offset);
		fprintf(f3, "%d\n", MEMORY[frame_number*256+offset]);
	}
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
	
	addressParsing(argv[1]);

	float page_fault_rate = ((float)page_faults / total_refs);
	float tlb_hit_rate = ((float)tlb_hits / total_refs);

	printf("Page faults = %d / %d, %f\n", page_faults, total_refs, page_fault_rate);
	printf("TLB hit rate: %d / %d, %f\n", tlb_hits, total_refs, tlb_hit_rate);

	return 0;
}
