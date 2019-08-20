#include <stdio.h>

int howManyBits(int x);

int main() {
	int x = 0x80000000;
	printf("%x\n", howManyBits(x));
}

int howManyBits(int x) {
  int b16, b8, b4, b2, b1, b0;
  int sign = x >> 31; // sign = 0xffffffff or 0x00000000
  // if x is positive, then don't change x, otherwise let x be ~x
  x = (sign&~x) | (~sign&x);

  b16 = !!(x>>16)<<4; // if !!(x>>16) == 1, then we know bits of x must more than 16
  x = x>>b16; // if b16 == 0x1111, we shift x to right 16 bits.
  b8 = !!(x>>8)<<3; // same as above
  x = x>>b8;
  b4 = !!(x>>4)<<2;
  x = x>>b4;
  b2 = !!(x>>2)<<1;
  x = x>>b2;
  b1 = !!(x>>1);
  x = x>>b1;
  b0 = x;
  return b16+b8+b4+b2+b1+b0+1; // +1 means plus sign
}
