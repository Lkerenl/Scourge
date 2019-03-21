#include "string.h"
#include "stdio.h"
int main(int argc, char const *argv[])
{
  char * ptr = NULL;
  const char str[] = "akl;jsdklajskdjkfjklasjdklajsldkj alkjsd aksdjla;jd kajldk alsjd kajdjkfghkdhfikasd substring tester kalsjdkl jfjald jkjd klajkldjlfjaklsjdlkad";
  const char substr[] = "substring tester";

  ptr = strstr(str, substr);
  printf("str: %p\nptr : %p %c\noffset : %ld\n", str, ptr, *ptr, ptr-str);
  return 0;
}
