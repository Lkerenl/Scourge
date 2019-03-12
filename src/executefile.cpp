#include "pch.h"
#include "executefile.h"
#include <stdio.h>
#include "libelfmaster/include/libelfmaster.h"

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
  unsigned long file_size = 0;
  conf->obj = CreateFile((char *)conf->target,
                          GENERIC_READ,
                          FILE_SHARE_WRITE|FILE_SHARE_READ,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
  if (conf->obj == INVALID_HANDLE_VALUE)
  {
    fprintf(stderr, "file open faild <INVALID_HANDLE_VALUE>\n",);
    return false;
  }

  file_size = GetFileSize(conf->obj, 0);
  if (file_size == 0)
  {
    fprintf(stderr, "file size error\n", );
    return false;
  }

  return true;



  #elif _WIN64
  #elif __gnu_linux__

  elfobj_t obj;
  elf_error_t error;
  unsigned int count = 0;

  if (elf_open_object((char *) conf->target,
                      &obj,
                      ELF_LOAD_F_MODIFY,
                      &error) == false)
  {
    fprintf(stderr, "%s\n", elf_error_msg(&error));
    return false;
  }

  conf->obj = obj;


  #endif

  return true;
}
