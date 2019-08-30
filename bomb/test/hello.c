#include <stdio.h>

int func4(int x, int si, int dx) {
  // x, si=0, dx = 0
  int eax = ((dx-si)+((dx-si)>>31)) >> 1;
  int cx = (((dx-si)+((dx-si)>>31)) >> 1) + si;

  if (cx <= x) {
    eax = 0;
    if (cx >= x) {
      return eax;
    } else {
      eax = func4(x, cx+1, dx);
      return eax*2 + 1;
    }
  } else {
    eax = func4(x, si, cx - 1) ;
    return eax*2;
  }
}


int main(void)
{
	for (int i = 0; i<= 0xe; i++) {
		if (func4(i, 0, 0xe) == 0) {
			printf("%d\n",i);
		}
	}
	return 0;
}
