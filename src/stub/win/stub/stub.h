#pragma once

#ifdef STUB_EXPORTS
#define STUB_API __declspec(dllexport)
#else
#define STUB_API __declspec(dllimport)
#endif // STUB_EXPORTS

#include <windows.h>
extern "C"
{
	typedef struct _GLOBAL_PARAM
	{
		DWORD start;
		BOOL show_message;
		ULONGLONG image_base;
		DWORD oep;
		PBYTE start_va;
		DWORD code_szie;
		BYTE xor_char;
	}global_param_t;

	typedef ULONGLONG(WINAPI * fnc_get_proc_addr)(_In_ HMODULE module, _In_ LPCSTR proc_name);
	typedef HMODULE(WINAPI *fnc_load_libraryA)(_In_ LPCSTR lib_filename);
	typedef int (WINAPI * fnc_message_box)(HWND, LPCSTR, LPSTR, UINT);
	typedef HMODULE(WINAPI *fnc_module_handleA)(_In_opt_ LPCSTR module_name);
	typedef HMODULE(WINAPI *fnc_virtual_protect)(_In_ LPVOID address, _In_ size_t size, _In_ DWORD new_protect, _Out_ PDWORD old_protect);
	typedef void (WINAPI *fnc_exit_procrss)(_In_ UINT exit_code);

	void start();
}
