#include "pch.h"
#include "executefile.h"
#include <stdio.h>

#ifndef _WIN32
#include <errno.h>
#endif

bool open_file(struct _CONFIG * const conf)
{
  // unsigned int bytes_readed = 0;

  // struct Fhdr fhdr;
  // uint64_t len;
  // uint8_t * buf;

  #ifdef _WIN32
  errno_t err =  fopen_s(conf->fp, (char *) conf->target, "rb");
  if (err != 0)
  {
    fprintf(stderr, "%s\n", perror(err));
    return false;
  }
  #else
  conf->fp = fopen((char *) conf->target, "rb");
  if(errno != 0)
  {
    fprintf(stderr, "\033[31m[-] %s: %s\033[0m\n", conf->target, strerror(errno));
    return false;
  }

  #endif

  return true;
}
