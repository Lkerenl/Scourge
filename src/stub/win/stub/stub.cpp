// stub.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "stub.h"

#pragma comment(linker, "/merge:.data=.text")
#pragma comment(linker, "/merge:.rdata=.text")
#pragma comment(linker, "/section:.text=,RWE")

extern "C" __declspec(dllexport) global_param_t stc_param = { (DWORD)(start) };

typedef void(*func) ();
func g_oep;

ULONGLONG get_kernel32_addr()
{
	ULONGLONG kernel32_addr = 0;
	_TEB * teb = NtCurrentTeb();
	PULONGLONG peb = (PULONGLONG)*(PULONGLONG)((ULONGLONG)teb + 0x60);
	PULONGLONG ldr = (PULONGLONG)*(PULONGLONG)((ULONGLONG)peb + 0x18);
	PULONGLONG in_load_order_module_list = (PULONGLONG)((ULONGLONG)ldr + 0x10);
	PULONGLONG module_exe = (PULONGLONG)*in_load_order_module_list;
	PULONGLONG module_nt_dll = (PULONGLONG)*module_exe;
	PULONGLONG module_kernel32 = (PULONGLONG)*module_nt_dll;
	kernel32_addr = module_nt_dll[6];
	return kernel32_addr;
}

ULONGLONG get_proc_address()
{
	ULONGLONG base = get_kernel32_addr();
	PIMAGE_DOS_HEADER p_image_dos_header = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS64 p_nt = (PIMAGE_NT_HEADERS64)p_image_dos_header->e_lfanew;
	PIMAGE_DATA_DIRECTORY p_export_dir = p_nt->OptionalHeader.DataDirectory;
	p_export_dir = &(p_export_dir[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	DWORD offset = p_export_dir->VirtualAddress;
	PIMAGE_EXPORT_DIRECTORY p_export = (PIMAGE_EXPORT_DIRECTORY)(base + offset);
	DWORD fun_count = p_export->NumberOfFunctions;
	DWORD fun_name_count = p_export->NumberOfNames;
	DWORD mod_offset = p_export->Name;

	PDWORD p_export_addr_table = (PDWORD)(base + p_export->AddressOfFunctions);
	PDWORD p_export_name_table = (PDWORD)(base + p_export->AddressOfNames);
	PWORD p_export_index_table = (PWORD)(base + p_export->AddressOfNameOrdinals);

	for (DWORD ordinal = 0; ordinal < fun_count; ordinal++)
	{
		if (!p_export_addr_table[ordinal])
			continue;
		DWORD id = p_export->Base + ordinal;
		ULONGLONG fun_addr_offset = p_export_name_table[ordinal];

		for (DWORD index = 0; index < fun_name_count; index++)
		{
			if (p_export_index_table[index] == ordinal)
			{
				ULONGLONG name_offset = p_export_name_table[index];
				char * fun_name = (char *)((ULONGLONG)base + name_offset);
				if (!strcmp(fun_name, "GetProcAddress"))
				{
					return base + fun_addr_offset;
				}
			}
		}
		return 0;
	}
}

void decrypt_code()
{
	PBYTE base = (PBYTE)((ULONGLONG)stc_param.image_base + stc_param.start_va);
	for (DWORD i = 0; i < stc_param.code_szie; i++)
	{
		base[i] ^= stc_param.xor_char;
	}
}

void start()
{
	fnc_get_proc_addr p_get_proc_address = (fnc_get_proc_addr)get_proc_address();
	ULONGLONG base = get_kernel32_addr();
	fnc_load_libraryA p_load_libraryA = (fnc_load_libraryA)p_get_proc_address((HMODULE)base, "LoadLibraryA");
	fnc_module_handleA p_module_handleA = (fnc_module_handleA)p_get_proc_address((HMODULE)base, "GetModuleHandleA");
	fnc_virtual_protect p_virtual_protect = (fnc_virtual_protect)p_get_proc_address((HMODULE)base, "VirtualProtect");
	HMODULE user32 = (HMODULE)p_module_handleA("user32.dll");
	fnc_message_box p_message_box = (fnc_message_box)p_get_proc_address(user32, "MessageBoxA");
	HMODULE kernel32 = (HMODULE)p_module_handleA("kernel32.dll");
	fnc_exit_procrss p_exit_process = (fnc_exit_procrss)p_get_proc_address(kernel32, "ExitProcess");

	ULONGLONG code_base = stc_param.image_base + (DWORD)stc_param.start_va;
	DWORD old_protect = 0;
	p_virtual_protect((LPBYTE)code_base, stc_param.code_szie, PAGE_EXECUTE_READWRITE, &old_protect);
	decrypt_code();
	g_oep = (func)(stc_param.image_base + stc_param.oep);
	g_oep();

	p_exit_process(0);
}
