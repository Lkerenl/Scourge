#include "pch.h"
#include "string.h"
#include "stdio.h"

int main(int argc, char const *argv[]) {
  uint8_t a[1024] = "l;kasjklfsjlkas;jdfkljsafjlkjfklds\xaclasajdl\x00klajsaaabbbcccdlkasd";
  uint8_t sub[20] = "aaabbbccc";
  uint8_t * target = mem_search(a,sub,1024,20);
  printf("%p,%s\n",target,target);

  return 0;
}
