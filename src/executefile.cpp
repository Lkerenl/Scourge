#include "pch.h"
#include "executefile.h"

#ifndef _WINDOWS_

#elif __gnu_linux__

bool open_file()
{
  conf_init();
  struct Fhdr fhdr;
  uint64_t len;
  uint8_t * buf;
  conf.target = tg.target;
  conf.fp = fopen(target,"rb");
  if (conf.fp == NULL)
  {
    fprintf(stderr, "file open error.\n");
    return -1;
  }

  readelf(conf.fp, &fhdr);
  printelfhdr(&fhdr);
}


#else

bool open_file()
{
  char * target = tg.target;
  unsigned int bytes_readed = 0;

}


#endif
