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

#ifdef _WIN32
  #include <tchar.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #ifdef __gnu_linux__
    #include <elf.h>
  #endif
#endif

typedef struct _Marking_interval
{
  size_t begin;
  size_t end;
} mi_t;

struct _CONFIG
{
	int process;
	void * target;
  void * obj;
  // mi_t marked[0];
  mi_t * marked;
};


inline void conf_init(struct _CONFIG * conf)
{
  conf->process = 0;
  conf->target = NULL;
  conf->obj = NULL;
  conf->marked = NULL;
}


#define FAILED "\033[31m[-]\033[0m"

#define BEGIN_FALGS "\xEB\x01\x53\x63\x6f\x75\x72\x67\x65\x20\x42\x65\x67\x69\x6e"
#define END_FLAGS "\xEB\x0F\x53\x63\x6f\x75\x72\x67\x65\x20\x45\x6e\x64"

uint8_t * mem_search(const uint8_t *, const uint8_t *, const uint64_t, const uint64_t);


#endif //PCH_H
