#include <stdio.h>
int main(){
	unsigned char a = 1;
	unsigned char* b = &a;
	printf("%d\n", b);
	printf("%x\n", b);
	printf("%d\n", *b);
	printf("\n");

	int c = 1;
	printf("%x\n", *(unsigned char*)&c);
}