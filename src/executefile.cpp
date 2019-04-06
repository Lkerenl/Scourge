#include "pch.h"
#include "executefile.h"


#include <stdio.h>

#ifndef _WIN32
#include <errno.h>
#endif

enum class AddressType {
  PhysicalOffset,
  RelativeVirtualAddress,
  VirtualAddress
};

static inline bool get_ldd_info(elfobj_t *);

using ParsedPeRef =
    std::unique_ptr<peparse::parsed_pe, void (*)(peparse::parsed_pe *)>;

static ParsedPeRef openExecutableRef(const std::string &path) noexcept {
  // The factory function does not throw exceptions!
  ParsedPeRef obj(peparse::ParsePEFromFile(path.data()),
                  peparse::DestructParsedPE);
  if (!obj) {
    return ParsedPeRef(nullptr, peparse::DestructParsedPE);
  }

  return obj;
}

bool convertAddress(ParsedPeRef &pe,
                    std::uint64_t address,
                    AddressType source_type,
                    AddressType destination_type,
                    std::uint64_t &result) noexcept {
  if (source_type == destination_type) {
    result = address;
    return true;
  }

  std::uint64_t image_base_address = 0U;
  if (pe->peHeader.nt.FileHeader.Machine == peparse::IMAGE_FILE_MACHINE_AMD64) {
    image_base_address = pe->peHeader.nt.OptionalHeader64.ImageBase;
  } else {
    image_base_address = pe->peHeader.nt.OptionalHeader.ImageBase;
  }

  struct SectionAddressLimits final {
    std::uintptr_t lowest_rva;
    std::uintptr_t lowest_offset;

    std::uintptr_t highest_rva;
    std::uintptr_t highest_offset;
  };

  auto L_getSectionAddressLimits = [](void *N,
                                      peparse::VA secBase,
                                      std::string &secName,
                                      peparse::image_section_header s,
                                      peparse::bounded_buffer *data) -> int {
    static_cast<void>(secBase);
    static_cast<void>(secName);
    static_cast<void>(data);

    SectionAddressLimits *section_address_limits =
        static_cast<SectionAddressLimits *>(N);

    section_address_limits->lowest_rva =
        std::min(section_address_limits->lowest_rva,
                 static_cast<std::uintptr_t>(s.VirtualAddress));

    section_address_limits->lowest_offset =
        std::min(section_address_limits->lowest_offset,
                 static_cast<std::uintptr_t>(s.PointerToRawData));

    std::uintptr_t sectionSize;
    if (s.SizeOfRawData != 0) {
      sectionSize = s.SizeOfRawData;
    } else {
      sectionSize = s.Misc.VirtualSize;
    }

    section_address_limits->highest_rva =
        std::max(section_address_limits->highest_rva,
                 static_cast<std::uintptr_t>(s.VirtualAddress + sectionSize));

    section_address_limits->highest_offset =
        std::max(section_address_limits->highest_offset,
                 static_cast<std::uintptr_t>(s.PointerToRawData + sectionSize));

    return 0;
  };

  SectionAddressLimits section_address_limits = {
      std::numeric_limits<std::uintptr_t>::max(),
      std::numeric_limits<std::uintptr_t>::max(),
      std::numeric_limits<std::uintptr_t>::min(),
      std::numeric_limits<std::uintptr_t>::min()};

  IterSec(pe.get(), L_getSectionAddressLimits, &section_address_limits);

  switch (source_type) {
    case AddressType::PhysicalOffset: {
      if (address >= section_address_limits.highest_offset) {
        return false;
      }

      if (destination_type == AddressType::RelativeVirtualAddress) {
        struct CallbackData final {
          bool found;
          std::uint64_t address;
          std::uint64_t result;
        };

        auto L_inspectSection = [](void *N,
                                   peparse::VA secBase,
                                   std::string &secName,
                                   peparse::image_section_header s,
                                   peparse::bounded_buffer *data) -> int {
          static_cast<void>(secBase);
          static_cast<void>(secName);
          static_cast<void>(data);

          std::uintptr_t sectionBaseOffset = s.PointerToRawData;

          std::uintptr_t sectionEndOffset = sectionBaseOffset;
          if (s.SizeOfRawData != 0) {
            sectionEndOffset += s.SizeOfRawData;
          } else {
            sectionEndOffset += s.Misc.VirtualSize;
          }

          auto callback_data = static_cast<CallbackData *>(N);
          if (callback_data->address >= sectionBaseOffset &&
              callback_data->address < sectionEndOffset) {
            callback_data->result = s.VirtualAddress + (callback_data->address -
                                                        s.PointerToRawData);

            callback_data->found = true;
            return 1;
          }

          return 0;
        };

        CallbackData callback_data = {false, address, 0U};
        IterSec(pe.get(), L_inspectSection, &callback_data);

        if (!callback_data.found) {
          return false;
        }

        result = callback_data.result;
        return true;

      } else if (destination_type == AddressType::VirtualAddress) {
        std::uint64_t rva = 0U;
        if (!convertAddress(pe,
                            address,
                            source_type,
                            AddressType::RelativeVirtualAddress,
                            rva)) {
          return false;
        }

        result = image_base_address + rva;
        return true;
      }

      return false;
    }

    case AddressType::RelativeVirtualAddress: {
      if (address < section_address_limits.lowest_rva) {
        result = address;
        return true;
      } else if (address >= section_address_limits.highest_rva) {
        return false;
      }

      if (destination_type == AddressType::PhysicalOffset) {
        struct CallbackData final {
          bool found;
          std::uint64_t address;
          std::uint64_t result;
        };

        auto L_inspectSection = [](void *N,
                                   peparse::VA secBase,
                                   std::string &secName,
                                   peparse::image_section_header s,
                                   peparse::bounded_buffer *data) -> int {
          static_cast<void>(secBase);
          static_cast<void>(secName);
          static_cast<void>(data);

          std::uintptr_t sectionBaseAddress = s.VirtualAddress;
          std::uintptr_t sectionEndAddress =
              sectionBaseAddress + s.Misc.VirtualSize;

          auto callback_data = static_cast<CallbackData *>(N);
          if (callback_data->address >= sectionBaseAddress &&
              callback_data->address < sectionEndAddress) {
            callback_data->result =
                s.PointerToRawData +
                (callback_data->address - sectionBaseAddress);

            callback_data->found = true;
            return 1;
          }

          return 0;
        };

        CallbackData callback_data = {false, address, 0U};
        IterSec(pe.get(), L_inspectSection, &callback_data);

        if (!callback_data.found) {
          return false;
        }

        result = callback_data.result;
        return true;

      } else if (destination_type == AddressType::VirtualAddress) {
        result = image_base_address + address;
        return true;
      }

      return false;
    }

    case AddressType::VirtualAddress: {
      if (address < image_base_address) {
        return false;
      }

      std::uint64_t rva = address - image_base_address;
      return convertAddress(pe,
                            rva,
                            AddressType::RelativeVirtualAddress,
                            destination_type,
                            result);
    }

    default: { return false; }
  }
}

// bool open_file(struct _CONFIG * const conf)
// {
//
//
//   if (conf->type == PE)
//   {
//     conf->obj = CreateFile((char *)conf->target,
//                             GENERIC_READ,
//                             FILE_SHARE_WRITE|FILE_SHARE_READ,
//                             NULL,
//                             OPEN_EXISTING,
//                             FILE_ATTRIBUTE_NORMAL,
//                             NULL);
//
//     if (conf->obj == INVALID_HANDLE_VALUE)
//     {
//       fprintf(stderr, "%s file open faild <INVALID_HANDLE_VALUE>\n", FAILED);
//       return false;
//     }
//
//     file_size = GetFileSize(conf->obj, 0);
//     if (file_size == 0)
//     {
//       fprintf(stderr, "file size error\n", FAILED);
//       return false;
//     }
//
//     return true;
//
//   }
//   else
//   {
//     elfobj_t * obj = (elfobj_t *)malloc(sizeof(elfobj_t));
//     elf_error_t error;
//
//     if (elf_open_object((char *) conf->target,
//                         obj,
//                         ELF_LOAD_F_FORENSICS,
//                         &error) == false)
//     {
//       fprintf(stderr, "%s %s\n", FAILED, elf_error_msg(&error));
//       return false;
//     }
//
//     conf->obj = obj;
//   }
//   return true;
// }

bool open_file(struct _CONFIG * const conf)
{
  if (conf->type == PE)
  {
    peparse::parsed_pe * p = peparse::ParsePEFromFile(static_cast<char *> (conf->target));
    if(p == NULL)
    {
      fprintf(stderr, "%s pe file open error\n", FAILED);
      return false;
    }
    switch (p->peHeader.nt.OptionalMagic)
    {
      case peparse::NT_OPTIONAL_32_MAGIC: conf->arch = PE32;break;
      case peparse::NT_OPTIONAL_64_MAGIC: conf->arch = PE64;break;
    }

    conf->obj = (void *)p;
  }
  else
  {
    elfobj_t * obj = (elfobj_t *)malloc(sizeof(elfobj_t));
    elf_error_t error;

    if (elf_open_object((char *) conf->target,
                        obj,
                        ELF_LOAD_F_FORENSICS,
                        &error) == false)
    {
      fprintf(stderr, "%s %s\n", FAILED, elf_error_msg(&error));
      return false;
    }

    conf->obj = obj;
  }

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

bool get_oep(struct _CONFIG * const conf)
{
  if(conf->type == ELF)
  {
    elfobj_t * obj = (elfobj_t *)conf->obj;
    elf_section_iterator_t s_iter;
    struct elf_section section;
    size_t len;
    uint8_t * entry_point;
    uint64_t base_vaddr = 0, base_offset = 0;

    elf_section_iterator_init(obj,&s_iter);
    while(elf_section_iterator_next(&s_iter, &section) == ELF_ITER_OK)
    {
      if(strcmp(section.name ,".init") == 0)
      {
        base_vaddr = section.address;
        base_offset = section.offset;
        break;
      }
    }

    len = elf_text_filesz(obj) + elf_data_filesz(obj) - base_offset;
    entry_point = static_cast<uint8_t *>(elf_offset_pointer(obj, base_offset));
    // printf("elf:%#zx %#zx",(size_t)entry_point,len);
    conf->oep_info.oep_addr = static_cast<uint8_t *>(entry_point);
    conf->oep_info.len = len;
    return true;
  }
  else if(conf->type == PE)
  {
    std::string path = reinterpret_cast<char *>(conf->target);
    peparse::parsed_pe * p = (peparse::parsed_pe *) conf->obj;
    auto pe = openExecutableRef(path);
    peparse::VA entry_point;
    //printf("PE va:%#zx\n",entry_point);
    size_t result;
    if(peparse::GetEntryPoint(p, entry_point))
    {
      if(convertAddress(pe, entry_point,
          AddressType::VirtualAddress,
          AddressType::PhysicalOffset, result))
      {
        conf->oep_info.oep_addr = (uint8_t *)result;
      }
      else{
        fprintf(stderr, "conver address error\n");
        return false;
      }
      if(conf->arch == PE32)
        conf->oep_info.len = p->peHeader.nt.OptionalHeader.SizeOfCode;
      else
        conf->oep_info.len = p->peHeader.nt.OptionalHeader64.SizeOfCode;
      return true;
    }
    else
    {
      fprintf(stderr, "Error: %u (%s)\n", peparse::GetPEErr(),peparse::GetPEErrString().c_str());
      return false;
    }
  }

  return false;
}
