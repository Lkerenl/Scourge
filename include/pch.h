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
  uint8_t * begin;
  uint8_t * end;
} mi_t;
typedef struct _OEP_INFO
{
  uint8_t * oep_addr;
  size_t len;
}oep_info_t;
typedef enum _EXE_TYPE { UnJudged = 0,PE = 1, ELF = 2 }exe_t;
typedef enum _ARCH { Uk, PE32, PE64, X86ELF, X86_64_ELF }arch_t;
typedef enum _COMPRESSION_ALG { APLIB, LZ4 }com_t;
struct _CONFIG
{
	int process;
	void * target;
  void * obj;
  exe_t type;
  arch_t arch;
  oep_info_t oep_info;
  mi_t * marked;
  com_t comprs_alg;
  char key[32];
};



inline void conf_init(struct _CONFIG * conf)
{
  conf->process = 0;
  conf->target = NULL;
  conf->type = UnJudged;
  conf->arch = Uk;
  conf->obj = NULL;
  conf->marked = NULL;
  conf->oep_info = {0,0};
  conf->comprs_alg = APLIB;
  memset(conf->key, 0, 32);
}


#define FAILED "\033[31m[-]\033[0m"

#define BEGIN_FLAGS "\xEB\x01\x53\x63\x6f\x75\x72\x67\x65\x20\x42\x65\x67\x69\x6e"
#define END_FLAGS "\xEB\x0F\x53\x63\x6f\x75\x72\x67\x65\x20\x45\x6e\x64"
#define BEGIN_FLAGS_LEN 15
#define END_FLAGS_LEN 13
size_t mem_search(uint8_t *, uint8_t *, size_t, size_t);


#endif //PCH_H
