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

// index:page number
// value:(0:frame number, 1:page offset, 2:valid/invalid bit)
int PAGE_TABLE[256][3];
// index:nothing, value:(0:page number, 1:frame number)
int TLB[16][2];
// physical memory
int MEMORY[256*256];

	TAILQ_HEAD(tailhead, entry) head;
	struct tailhead *headp;
	struct entry {
		int val;
		TAILQ_ENTRY(entry) entries;
	} *n, *np;

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

// Set up the queue for FIFO.
struct tailhead* createQueue() {
	TAILQ_INIT(&head);
	n = malloc(sizeof(struct entry));
	TAILQ_INSERT_TAIL(&head, n, entries);

	return (headp);
}


void pageFault(int pageNumber, struct tailhead* headPointer) {
	FILE* file;
	// This should be the first index of our desired page
	int beginIndex = 256 * pageNumber;
	int maxChar = beginIndex + 256;
	char str[maxChar];

	// Open the .bin file
	file = fopen("BACKING_STORE.bin", "rb");

	if (file == NULL) {
		printf("Could not open BACKING_STORE.bin\n");
		return;
	}

	// Read in the data from BACKING_STORE.bin	
	fread(str, 1, maxChar, file);

	// Bring 256 items from range (beginIndex, maxChar) into physical memory
	for (int i = beginIndex; i < maxChar; i++) {
		MEMORY[i] = str[i];
	}
	
	// Close the file when finished
	fclose(file);

	// Update the queue

	// Update the TLB


	return;
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

void addressParsing(char *f, struct tailhead* headPointer) {	
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
				// Read from memory.
				pageFault(number, headPointer);
			}
		}
		// Write info to the output files.
		fprintf(f1, "%d", address);
		fprintf(f2, "%d", number);
		//fprintf(f3, "%d", SIGNED_BYTE_VALUE);
	}
	fclose(file);
	fclose(f1);
	fclose(f2);
	fclose(f3);
	return;
}

int main(int *argc, char **argv) {	
	struct tailhead* headPointer = createQueue();

	addressParsing(argv[1], headPointer);

	return 0;
}
