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

void addressParsing(char *f) {
	FILE* file;
	int maxChar = 10;
	char str[maxChar];
	// open txt file containing all logical addresses
	file = fopen("addresses.txt", "r");
	
	// return if file could not open
	if (file == NULL) {
		printf("Could not open addresses.txt\n");
		return;
	}

	// parse and get info from each logical address
	while (fgets(str, maxChar, file) != NULL) {
		int address = atoi(str);
		int number = page_number(address);
		int offset = page_offset(address);
		//printf("Number: %d Offset: %d\n", number, offset);
		
		// Search the TLB for the page.
		frame_number = search_TLB(number);
		if (frame_number < 0) {
			// Search page table for page.
			frame_number = search_table(number);
			if (frame_number < 0)
				// Read from memory.
		}
	}
	return;
}

int main(int *argc, char **argv) {	
	addressParsing(argv[1]);
	return 0;
}
