#include <stdio.h>

int isTmax(int x);

int main() {
	int x = 0x7fffffff;
	isTmax(x);
}

int isTmax(int x) {
  //return !(((1<<31)^x)+1); // illegal ops <<
	printf("%x\n", x+1);
	printf("%x\n", (x+1)^x);
	printf("%x\n", !((x+1)&x));
	// printf("%x\n", x+1);
	// printf("%x\n", x+1);
	
	

	return !(((x+1)^x)+ !((x+1)^x));
}
