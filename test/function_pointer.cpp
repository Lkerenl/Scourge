#include <stdio.h>
typedef void (__stdcall *func_p) (int);

void hello(int a)
{
  printf("%d\n",a);
}
int main(int argc, char const *argv[]) {
  func_p a = &hello;
  a(3);
  return 0;
}
