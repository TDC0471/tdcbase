#pragma once
#include <Windows.h>

namespace util {
	PVOID GetModuleHandleA(LPCSTR);
	PVOID GetModuleHandleW(LPCWSTR);

	FARPROC GetProcAddress(PVOID, LPCSTR);
}
