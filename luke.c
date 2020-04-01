/*
   Jack Taylor and Luke Kurlandski
   Dr. Yoon
   CSC345: Operating Systems
   Project 3: Virtual Memory
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// index = page number, value = page offset, valid/invalid
int PAGE_TABLE[256][2];
int TLB[16][2];

// Calculate the page number from a logical address.
int page_number(int address) {
	// 65280 = 000000001111111100000000
	return (65280 & address);
}

// Calculate the offset for a logical address.
int page_offset(int address) {
	// 255 = 000000000000000011111111
	return (255 & address);
}

// Return a 0 if the page number is in the TLB.
int search_TLB(int number, int offset) {
	for (int i=0; i<16; i++) {
		if (TLB[i][0] == number && TLB[i][0] == offset)
			return 0;
	}
	return -1;
}

// Return a 0 if the page number is in the page table.
int search_table(int number, int offset) {
	if (PAGE_TABLE[number][1] == 1) 
		return 0;
	else 
		return -1;
}

void addressParsing() {
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
		// Search the TLB for the page.
		if (search_TLB(number, offset) < 0) {
			// Not found in TLB, so we search page table.
			if (search_table(number, offset) < 0) {
				// Not found in TLB, so read from memory.
			}
		}
	}

	return;
}

int main(int *argc, char **argv) {
	printf("%d\n", page_number(1));
	return 0;
}


