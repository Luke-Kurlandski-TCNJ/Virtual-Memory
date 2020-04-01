/*
   Jack Taylor and Luke Kurlandski
   Dr. Yoon
   CSC345: Operating Systems
   Project 3: Virtual Memory
*/

#include <stdlib.h>
#include <stdio.h>

int page_number(int address) {
	return (65280 & address); // 65280 == 00..001111111100..00
}

int offset(int address) {
	return (255 & address); // 255 == 00...0011111111
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
	while (fgets(str, maxChar, file) != NULL) {
		int currAddress = atoi(str);
		printf("page number = %d\n", page_number(currAddress));
		printf("offset = %d\n", offset(currAddress));
	}

	return;
}

int main(int *argc, char **argv) {
	addressParsing();

	printf("%d\n", page_number(1));
	return 0;
}
