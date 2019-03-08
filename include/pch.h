#pragma once

#ifndef PCH_H
#define PCH_H

#define VP_NAME "Scourge"
#define VP_VERSION "0.0.1"
#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef _WINDOWS_
  #include <tchar.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #ifdef __gnu_linux__
    #include <elf.h>
  #endif
#endif

struct _CONFIG
{
	int process;
	void * target;
  FILE * fp;
  size_t fd;
};



inline void conf_init(struct _CONFIG * conf)
{
  conf->process = 0;
  conf->target = NULL;
  conf->fp = NULL;
  conf->fd = 0;
}


#endif //PCH_H
