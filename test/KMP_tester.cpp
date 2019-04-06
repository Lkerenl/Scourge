#include <string.h>
#include <stdio.h>
#include <stdlib.h>

size_t KMP_search(char *, char *, size_t, size_t);

int main(int argc, char const *argv[]) {
  char * str = (char *)"akljsdklajsdkljlkajdkl\0jaklsjdajskljdakljdkljaaaabbbbccccaljsdalksjdklaaaabbbccdajshdkjahsdjkhasldjhaljkhdkasd";
  char * substr = (char *)"aaaabbbccd";
  printf("find offset %zu\n", KMP_search(str,substr,110,strlen(substr)));
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
