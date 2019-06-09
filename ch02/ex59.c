#include <stdio.h>

int main(void){
	int x = 0x89abcdef; //int type
	int y = 0x76543210;

	// when four bytes type x & (0xff), C compile will force one byte type(0xff) trasform to four bytes type, which is ==> (0x000000ff)
	// so the (~0xff) ==> (0xffffff00), which is != (0x00)
	// if you limit the bytes number into only one, which would result ((char) (~0xff)) == (0x00)
	printf("%x\n", x & (0xff));
	printf("%x\n", y & (~0xff));
	printf("%x\n", x & (0xff) |y & (~0xff));

	return (x & (0xff) | y & (~0xff));

}