#pragma once
#ifndef EXEC_FILE
#define EXEC_FILE
#include "pch.h"
#include <iostream>
#include <limits>
#include <string>
#include <memory>
#include <algorithm>
#include <parser-library/parse.h>
#ifdef __cplusplus
extern "C"{
#endif
  #include "libelfmaster.h"
#ifdef __cplusplus
}
#endif

// #ifdef _WINDOWS_
//   #include <windows.h>
// #elif _linux_
//   #include <elf.h>
// #else
// #endif


bool open_file(struct _CONFIG * const);
void show_exe_info(elfobj_t *);
bool get_oep(struct _CONFIG * const);
// bool get_text_ptr(elfobj_t *, uint8_t **, uint64_t *, uint64_t *, uint64_t *);




#endif
