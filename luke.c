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
int PAGE_TABLE = [pow(2,8)];
int TLB = [16];



int page_number(int address) {
	// 65280 = 000000001111111100000000
	return (65280 & address);
}

int offset(int address) {
	// 255 = 000000000000000011111111
	return (255 & address);
}

int main(int *argc, char **argv) {
	printf("%d\n", page_number(1));
	return 0;
}


