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

int main(int *argc, char **argv) {
	printf("%d\n", page_number(1));
	return 0;
}


