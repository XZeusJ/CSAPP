#include <stdio.h>

int isTmax(int x);

int main() {
	// int x = 0x7fffffff;
	int x = 0x80000000;
	printf("%x\n", isTmax(x));
}

int isTmax(int x) {
  //return !(((1<<31)^x)+1); // illegal ops <<
	printf("%x\n", x^0x30);
	printf("%x\n", !(x^0x30));
	printf("%x\n", !(x^0x30) & !(x^0x32));
	printf("%x\n", !(!(x^0x30) & !(x^0x32)));
	// printf("%x\n", x+1);
	
	printf("test %x\n", -1>>31);
	//printf("%x\n", -1>>32);


  	return !(!(x^0x30) | !(x^0x31) | !(x^0x32) | !(x^0x33) | !(x^0x34) | !(x^0x35) | !(x^0x36) | !(x^0x37) | !(x^0x38) | !(x^0x39));
}
