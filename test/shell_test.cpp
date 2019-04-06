#include <parser-library/parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char const *argv[]) {
  if (argc < 3)
  {
    printf("Usage: shell_tester <filename> <packedname>\n");
    return -1;
  }
  parsed_pe * p = ParsePEFromeFile(argv[1])
  if(!p)
  {
    fprintf(stderr, "%zu: %s\nLocation :%s\n", GetPEErr(), GetPEErrString().c_str(),GetPEErrLoc());
    return -1;
  }


  return 0;
}


// bool save_extra_data(parsed_pe)
