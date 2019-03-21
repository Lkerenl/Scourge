#include "pch.h"

static inline int * get_next(uint8_t *, int);

uint8_t * mem_search(const uint8_t * ptr,const uint8_t * submem, const uint64_t size, const uint64_t sub_size)
{
  // uint8_t * p =
  int i,j;
  int * next = get_next((uint8_t *)submem, sub_size);
  for (i = 0, j = 0; i < size && j<sub_size ; i += j , j=0)
  {
    if( j == -1 || ptr[i] == submem[j])
      i++,j++;
    else
      j = next[j];
  }
  free(next);
  next = 0;
  if(j == sub_size)
    return (uint8_t *)&ptr[i-j];

  return NULL;
}

static inline int * get_next(uint8_t * ptr, int size)
{
  int * next_list = (int *) calloc(size, sizeof(int));
  int i = 0,j = -1;
  for(next_list[0] = -1; i < size;)
  {
    if(j == -1 || ptr[i] == ptr[j])
      next_list[++i] = ++j;
    else
      j = next_list[j];

  }

  for (i = 0;i<size;i++)
  {
    printf("%d ",next_list[j]);
  }

  return next_list;
}
