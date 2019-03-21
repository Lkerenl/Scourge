#include <stdio.h>
#include <capstone/capstone.h>

extern "C" {
  #include "libelfmaster.h"
}

int main(int argc, char ** argv)
{
  elfobj_t obj;
  elf_error_t error;
  struct elf_section s;

  if(argc < 2)
  {
    printf("Usage: get_text_tester filename\n");
    return -1;
  }

  if (elf_open_object(argv[1], &obj,
      ELF_LOAD_F_MODIFY | ELF_LOAD_F_FORENSICS, &error) == false)
  {
      fprintf(stderr, "%s\n", elf_error_msg(&error));
      return -1;
  }
  elf_section_by_name(&obj, ".text", &s);
  printf("name: %s\n",s.name);
  printf("type: %#x\n",s.type);
  printf("link: %#x\n",s.link);
  printf("info: %#x\n",s.info);
  printf("flags: %#lx\n",s.flags);
  printf("align: %#lx\n",s.align);
  printf("entsize: %#lx\n",s.entsize);
  printf("offset: %#lx\n",s.offset);
  printf("address: %#lx\n",s.address);
  printf("size: %#lx\n",s.size);
  elf_close_object(&obj);
}
