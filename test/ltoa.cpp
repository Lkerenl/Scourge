#include <stdlib.h>
#include <stdio.h>
int main()
{
  char buf[33] = {0};
  printf("%s\n", ltoa(123456L, buf, 10));
  return 0;
}
