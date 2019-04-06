#include "pch.h"


size_t mem_search(uint8_t * mem, uint8_t * submem, size_t memlen,size_t submem_len)
{
  int sm_len = (int)submem_len;
  int m_len = (int)memlen;
  int i,j;
  if (sm_len <=0 ||m_len <= 0)
  {
    fprintf(stderr,"len error\n");
    return -1;
  }
  if(sm_len > m_len)
  {
    fprintf(stderr, "sub > len\n" );
    return 0;
  }

  unsigned int * next = (unsigned int *)calloc(sm_len, sizeof(unsigned int));
  for(i = -1,j = 0,next[0] = -1; j<sm_len-1;)
  {
    if(i == -1 || submem[j] == submem[i])
      next[j++] = i++;

    else
      i = next[i];

  }

  for(j = 0,i = 0; i<m_len && j<sm_len; )
  {
    if (j == -1 ||mem[i] == submem[j])
      i++,j++;
    else
      j = next[j];
  }
  free(next);
  next = NULL;

  if(j == sm_len)
    return i-j;

  return -1;
}
