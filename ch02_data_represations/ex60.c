#include <stdio.h>

unsigned replace_byte(unsigned x, int i, unsigned char b) {
	// i*8 == i<<3
	return (x & ~(0xff << i*8)) | (b << i*8);
}

int main(void){
	int i = 1;
	printf("%x\n", 0x12345678 & ~(0xff << i*8));

	printf("%x\n", replace_byte(0x12345678, 2, 0xAB));
	printf("%x\n", replace_byte(0x12345678, 0, 0xAB));
}