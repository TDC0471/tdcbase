#include "ManualLibrary.h"
#include <intrin.h>
#include <winternl.h>
#include <atlbase.h>

inline PEB* getPeb()
{ 
#ifdef _WIN64
	return reinterpret_cast<PEB*>(__readgsqword(0x60));
#else
	return reinterpret_cast<PEB*>(__readfsdword(0x30));
#endif
}

bool wcscmpncend(const wchar_t* str1, const wchar_t* str2)
{
	size_t len1 = wcslen(str1);
	size_t len2 = wcslen(str2);

	if (len1 < len2)
		return false;

	for (size_t i = 0; i < len2; ++i)
	{
		if (towlower(str1[len1 - len2 + i]) != towlower(str2[i]))
			return false;
	}

	return true;
}

PVOID util::GetModuleHandleA(LPCSTR str)
{
	return util::GetModuleHandleW(CA2W(str));
}

PVOID util::GetModuleHandleW(LPCWSTR str)
{
	PLIST_ENTRY pHead = getPeb()->Ldr->InMemoryOrderModuleList.Flink;
	
	while (pHead->Flink != getPeb()->Ldr->InMemoryOrderModuleList.Flink)
	{
		PLDR_DATA_TABLE_ENTRY currentModuleEntry = CONTAINING_RECORD(pHead, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (wcscmpncend(currentModuleEntry->FullDllName.Buffer, str))
		{
			return currentModuleEntry->DllBase;
		}

		pHead = pHead->Flink;
	}

	return nullptr;
}

FARPROC util::GetProcAddress(PVOID hModule, LPCSTR str)
{
	PIMAGE_DOS_HEADER dosHeader = static_cast<PIMAGE_DOS_HEADER>(hModule);

	if (dosHeader == nullptr || dosHeader->e_magic != 0x5A4D)
	{
		return nullptr;
	}

	DWORD_PTR hModuleBase = reinterpret_cast<DWORD_PTR>(hModule);
	PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(hModuleBase + dosHeader->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY ExportDirectory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(hModuleBase + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	DWORD* nameTable = reinterpret_cast<DWORD*>(hModuleBase + ExportDirectory->AddressOfNames);

	for (int i = 0; i < ExportDirectory->NumberOfNames; i++)
	{
		const char* name = reinterpret_cast<const char*>(reinterpret_cast<DWORD_PTR>(hModule) + nameTable[i]);
		if (strcmp(str, name) == 0)
		{
			DWORD* functionTable = reinterpret_cast<DWORD*>(hModuleBase + ExportDirectory->AddressOfFunctions);
			WORD* ordinalTable = reinterpret_cast<WORD*>(hModuleBase + ExportDirectory->AddressOfNameOrdinals);
			DWORD* ordinalBase = reinterpret_cast<DWORD*>(hModuleBase + ExportDirectory->Base);
			//std::cout << ordinalBase << std::endl;

			return reinterpret_cast<FARPROC>(functionTable[ordinalTable[i] - (*ordinalBase - 1)] + reinterpret_cast<DWORD_PTR>(hModule));
		}
	}
	return nullptr;
}