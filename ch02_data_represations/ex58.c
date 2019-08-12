#include <stdio.h>

int main(void) {
	int a = 1;
	// size_t len = sizeof(char);

	// why we not use unsigned int*?  becuase it has 4 bytes to show
	// char only take 1 bytes to show each time
	// so use unsigned char*, we can show each bytes for whatever lenth of data type is
	unsigned char* start = (unsigned char*) &a; // get addr of a


	printf("%x\n", start[0]); // *start == start[0], which return the value in the first addr.

	return *start; // same as start[0]
}