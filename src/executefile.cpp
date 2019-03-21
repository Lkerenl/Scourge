#include "pch.h"
#include "executefile.h"


#include <stdio.h>

#ifndef _WIN32
#include <errno.h>
#endif



static inline bool get_ldd_info(elfobj_t *);



bool open_file(struct _CONFIG * const conf)
{
  // unsigned int bytes_readed = 0;

  // struct Fhdr fhdr;
  // uint64_t len;
  // uint8_t * buf;

  #ifdef _WIN32
  // unsigned long file_size = 0;
  conf->obj = CreateFile((char *)conf->target,
                          GENERIC_READ,
                          FILE_SHARE_WRITE|FILE_SHARE_READ,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

  if (conf->obj == INVALID_HANDLE_VALUE)
  {
    fprintf(stderr, "%s file open faild <INVALID_HANDLE_VALUE>\n", FAILED);
    return false;
  }

  file_size = GetFileSize(conf->obj, 0);
  if (file_size == 0)
  {
    fprintf(stderr, "file size error\n", FAILED);
    return false;
  }

  return true;



  #elif _WIN64
  #elif __gnu_linux__

  elfobj_t * obj = (elfobj_t *)malloc(sizeof(elfobj_t));
  elf_error_t error;
  //unsigned int count = 0;

  if (elf_open_object((char *) conf->target,
                      obj,
                      ELF_LOAD_F_FORENSICS,
                      &error) == false)
  {
    fprintf(stderr, "%s %s\n", FAILED, elf_error_msg(&error));
    return false;
  }

  conf->obj = obj;


  #endif

  return true;
}

void show_exe_info(elfobj_t * obj)
{
  char buf[128] = {0};
  #ifdef __gnu_linux__

  switch (obj->arch) {
    case i386:snprintf(buf, 128,"%-30s %s", "Arch: ", "Intel 80386");break;
    case x64:snprintf(buf, 128,"%-30s %s", "Arch: ", "Advanced Micro Devices X86-64");break;
    case unsupported:snprintf(buf,128,"%-30s %s", "Arch: ", "Unsupported architecture");
  }
  printf("%s\n",buf);
  memset(buf,0,128);

  // switch (obj->type)
  // {
  //   case ELF_EXEC:snprintf(buf, 128,"%-30s %s", "Type: ", "EXEC(Exectuable file)");break;
  //   case ELF_DYN:snprintf(buf, 128,"%-30s %s", "Type: ", "DYN(Shared object file)");break;
  // }

  printf("%s\n",buf);
  memset(buf,0,128);

  switch (obj->e_class) {
    case elfclass32:snprintf(buf, 128,"%-30s %s","Class: ","ELF32");break;
    case elfclass64:snprintf(buf, 128,"%-30s %s","Class: ","ELF64");break;
  }
  printf("%s\n",buf);

  printf("Ldd Info:\n");
  get_ldd_info(obj);

  printf("Number of sections: %zu\n", elf_section_count(obj));

  #elif _WIN32
  printf("TODO: Show WINDOWS Exectue File Info\n");
  #endif
}

static inline bool get_ldd_info(elfobj_t * obj)
{
  elf_error_t error;
  struct elf_shared_object so_object;
  struct elf_shared_object_iterator so_iter;

  if (elf_shared_object_iterator_init(obj, &so_iter, NULL,
    ELF_SO_RESOLVE_ALL_F, &error) == false)
  {
    fprintf(stderr, "%s elf_shared_object_iterator_init failed: %s\n",
            FAILED, elf_error_msg(&error));
    return false;
  }

  for (;;)
  {
    elf_iterator_res_t res;
    res = elf_shared_object_iterator_next(&so_iter, &so_object, &error);
    if (res == ELF_ITER_DONE)
      break;
    if (res == ELF_ITER_ERROR)
    {
      fprintf(stderr, "\n%s shared object iterator failed: %s\n",
          FAILED, elf_error_msg(&error));
      break;
    }
    if (res == ELF_ITER_OK || res == ELF_ITER_NOTFOUND)
    {
      printf("\t%-30s -->\t%s\n", so_object.basename, so_object.path);
    }
  }
  return true;
}

bool
get_text_ptr(elfobj_t * obj, uint8_t ** ptr, uint64_t * size,
              uint64_t * addr, uint64_t * offset)
{
  size_t len;
  uint64_t base_vaddr,base_offset;
  struct elf_section section;
  elf_section_iterator_t s_iter;

  elf_section_iterator_init(obj, &s_iter);
  while (elf_section_iterator_next(&s_iter, &section) == ELF_ITER_OK)
  {
    if(strcmp(section.name, ".init") == 0)
    {
      base_vaddr = section.address;
      base_offset = section.offset;
      break;
    }
  }

  len = elf_text_filesz(obj) + elf_data_filesz(obj);
  *ptr = (uint8_t *) elf_offset_pointer(obj, base_offset);
  *addr = base_vaddr;
  *offset = base_vaddr;
  *size = len - base_offset;

  return true;
}
