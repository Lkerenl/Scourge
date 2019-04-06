#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include "pch.h"
extern "C"
{
  #include "libelfmaster.h"
}
static bool search_marked(uint8_t * ptr, mi_t ** marked, size_t);

#define size_malloc(p) (*(((size_t *)p)-1) & ~(0x01|0x02|0x04))

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
  search_marked(ptr, &marked,1024);
  for (size_t i = 0; marked[i].begin != NULL ; i++) {
    printf("%p %p\n", marked[i].begin,marked[i].end);
  }
  return 0;
}

size_t KMP_search(char * str, char * substr, size_t memlen,size_t submem_len)
{
  int ss_len = (int)submem_len;
  int s_len = (int)memlen;
  int i,j;
  if (ss_len <=0 ||s_len <= 0)
  {
    fprintf(stderr,"len error\n");
    return -1;
  }
  if(ss_len > s_len)
  {
    fprintf(stderr, "sub > len\n" );
    return 0;
  }

  unsigned int * next = (unsigned int *)calloc(ss_len, sizeof(unsigned int));
  for(i = -1,j = 0,next[0] = -1; j<ss_len-1;)
  {
    if(i == -1 || substr[j] == substr[i])
      next[j++] = i++;

    else
      i = next[i];

  }

  for(j = 0,i = 0; i<s_len && j<ss_len; )
  {
    if (j == -1 ||str[i] == substr[j])
      i++,j++;
    else
      j = next[j];
  }
  free(next);
  next = NULL;

  if(j == ss_len)
    return i-j;

  return -1;
}

static bool search_marked(uint8_t * ptr, mi_t ** marked, size_t len_of_ptr)
{
	uint8_t * s_seek = ptr;
	uint8_t * mi_begin = NULL;
	uint8_t * mi_end = NULL;
  size_t len = len_of_ptr;
  int a = 0;
  mi_t * tmp = (mi_t *)malloc(sizeof(mi_t));

  do {
    size_t offset = 0;
    mi_end = mi_begin = NULL;
    if((offset = KMP_search((char *)s_seek, (char *)BEGIN_FLAGS,len,BEGIN_FLAGS_LEN)) != -1)
    {
      mi_begin = (uint8_t *)((size_t)s_seek + offset);
      s_seek += offset + BEGIN_FLAGS_LEN;
      len -= (offset + BEGIN_FLAGS_LEN);
    }
    if((offset = KMP_search((char*)s_seek,(char*)END_FLAGS,len_of_ptr,END_FLAGS_LEN)) != -1)
    {
      mi_end = (uint8_t *)((size_t)s_seek + offset);
      s_seek += offset + END_FLAGS_LEN;
      len -= (offset + END_FLAGS_LEN);
    }
  	if(mi_begin == NULL && mi_end)
  	{
  		fprintf(stderr, "%s The flags do not match(Not closed).  %p\n", FAILED, s_seek);
  		return false;
  	}
    printf("%p %p\n",mi_begin,mi_end);
    tmp[a].begin = mi_begin;
    tmp[a].end = mi_end;
    tmp = (mi_t *)realloc(tmp, malloc_usable_size(tmp) + sizeof(mi_t));
    tmp[++a].begin = NULL;
    tmp[a].end = NULL;
  } while(mi_begin || mi_end);

  printf("\n");
  for (size_t i = 0; tmp[i].begin != NULL ; i++) {
    printf("%p %p\n", tmp[i].begin,tmp[i].end);
  }
  *marked = tmp;
  return true;
}
