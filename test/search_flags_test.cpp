#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include "pch.h"
extern "C"
{
  #include "libelfmaster.h"
}
static bool search_marked(uint8_t * ptr, mi_t ** marked);
// struct _Marking_interval
// {
//   size_t begin;
//   size_t end;
// }mi_t;

#define size_malloc(p) (*(((size_t *)p)-1) & ~(0x01|0x02))

int main(int argc, char const *argv[]) {
  if (argc < 2)
  {
    printf("Usage: ./search_flags_tester <filename>\n");
    return -1;
  }
  size_t fd = 0;
  mi_t * marked = NULL;
  uint8_t * ptr = NULL;
  char * str[1024] = {0};
  ptr = (uint8_t *)str;
  fd = open(argv[1], O_RDONLY);
  read(fd, str, 1024);
  search_marked(ptr, &marked);

  return 0;
}

static bool search_marked(uint8_t * ptr, mi_t ** marked)
{
	uint8_t * s_seek = ptr;
	uint8_t * mi_begin = NULL;
	uint8_t * mi_end = NULL;
	const uint8_t bgstrsz = strlen(BEGIN_FALGS) + 1;
	const uint8_t edstrsz = strlen(END_FLAGS) + 1;
  mi_t * tmp = (mi_t *)malloc(sizeof(mi_t));
	do {
			//TODO : add marked

		mi_begin = mi_end = NULL;
		mi_begin = (uint8_t *)strstr((char *)s_seek, (char *)BEGIN_FALGS);
		if (mi_begin != NULL)
			{
        s_seek = mi_begin + bgstrsz;
        tmp->begin = (size_t)mi_begin;
      }

		mi_end = (uint8_t *)strstr((char *)s_seek, (char *)END_FLAGS);
		if (mi_end != NULL)
    {
			s_seek = mi_end + edstrsz;
      tmp->end = (size_t)mi_end;
    }

		if(mi_begin == NULL && mi_end)
		{
			fprintf(stderr, "%s The flags do not match(Not closed).  %p\n", FAILED, s_seek);
			return false;
		}
    printf("mi_begin: %p,mi_end: %p\n", mi_begin, mi_end);
    tmp = (mi_t *)realloc(tmp, size_malloc(tmp) + sizeof(mi_t));
	} while(mi_begin || mi_end);

  return true;
}
