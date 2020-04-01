/*
   Jack Taylor and Luke Kurlandski
   Dr. Yoon
   CSC345: Operating Systems
   Project 3: Virtual Memory
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// index = page number, value = page offset
int PAGE_TABLE [256];
int TLB [16];

int page_number(int address) {
	// 255 == 00..001111111100..00, shifts address 8 bits to the right
	return (255 & address>>8);
}

int offset(int address) {
	// 255 == 00...0011111111
	return (255 & address);
}

void pageFault(int pageNumber) {
	FILE* file;
	// This should be the first index of our desired page
	int beginIndex = 256 * pageNumber;
	int maxChar = beginIndex + 256; //TESTING: Set size to entire file
	unsigned char str[maxChar];

	// open the .bin file
	file = fopen("BACKING_STORE.bin", "rb");

	if (file == NULL) {
		printf("Could not open BACKING_STORE.bin\n");
		return;
	}

	// read in the data from BACKING_STORE.bin	
	fread(str, 1, maxChar, file);

	// TESTING: print out 256 items after beginIndex
	for (int i = beginIndex; i < maxChar; i++) {
		printf("%u  ", str[i]);
	}

	printf("\n");
	
	// close the file when finished
	fclose(file);

	return;
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

	// parse through each line, getting info from each logical address
	int currAddress;
	while (fgets(str, maxChar, file) != NULL) {
		currAddress = atoi(str);
		printf("page number = %d\n", page_number(currAddress));
		printf("offset = %d\n", offset(currAddress));
	
		pageFault(page_number(currAddress));
	}
	
	// close the file when finished
	fclose(file);

	return;
}

int main(int *argc, char **argv) {
	addressParsing();

	printf("%d\n", page_number(1));
	return 0;
}
