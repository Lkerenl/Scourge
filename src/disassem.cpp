#include "disassem.h"
#include "executefile.h"
#include <inttypes.h>

bool show_disassem_code(elfobj_t * obj)
{
  size_t count;
  csh handle;
  cs_insn * insn;

  size_t len;
  uint64_t base_vaddr, base_offset;
  uint8_t * ptr;
  struct elf_section s;
  elf_section_iterator_t s_iter;

  elf_section_iterator_init(obj, &s_iter);
  while (elf_section_iterator_next(&s_iter, &s) == ELF_ITER_OK)
  {
    if (strcmp(s.name, ".init") == 0)
    {
      base_vaddr = s.address;
      base_offset = s.offset;
      break;
    }
  }

  len = elf_text_filesz(obj) + elf_data_filesz(obj);
  ptr = (uint8_t *)elf_offset_pointer(obj, base_offset);
  // ptr = get_text_ptr(obj);
  if (ptr == NULL)
  {
    fprintf(stderr, "%s Unable to get offset: %#lx\n", FAILED, base_offset);
    return false;
  }

  switch (obj->e_class) {
    case elfclass32:
      if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK)
        return false;
      break;
    case elfclass64:
      if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return false;
      break;
  }
  cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);

  count = cs_disasm(handle, ptr, len-base_offset, base_vaddr, base_offset, &insn);
  if(count > 0)
  {
    size_t j;
    // bool checked = false;
    for (j = 0; j < count; j++)
    {
      const char * sname = ".unknown";
      const char * symname = "sub_unknown";
      struct elf_symbol symbol;

      if (elf_section_by_address(obj, insn[j].address, &s) == true)
      {
        sname = s.name;
        // printf("\nDisassembly of section %s:\n", sname);
      }

      if (elf_symbol_by_value(obj, insn[j].address, &symbol) == true )
      {
        symname = symbol.name;
        // printf("\n%#016lx: <%s>\n:", insn[j].address, symname);
      }


      // printf(" 0x%" PRIx64 ":\t%s\t\t%s", insn[j].address, insn[j].mnemonic, insn[j].op_str);
      // printf(" 0x%" PRIx64 ":\t%s\t\t%s\n", insn[j].address,insn[j].mnemonic, insn[j].op_str);
      printf("0x%" PRIx64 ":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic,
					insn[j].op_str);
    }
    cs_free(insn, count);
  }
  else
  {
    fprintf(stderr, "%s Failed to disassemble given code!\n", FAILED);
    return false;
  }

  cs_close(&handle);

  return true;
}
