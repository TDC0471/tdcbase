//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>
#include <stdio.h>
#include <dxgi.h>
#include <d3d11.h>
#include "memory.h"
#include <typeinfo>
#include <intrin.h>
#include "VEHHook.h"
#include "console.h"
#include "ShadowVmtHook.h"
#include "DxHooks.h"
#include "VEHD3D11Hooker.h"


DWORD WINAPI MainThread(HMODULE hmod)
{
#ifdef DEBUG
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

	c::printf("Hello from DLL!\n");

	DxHooks::initHooks<VEHD3D11Hooker>();

	//while f1 is not pressed
	while (!(GetAsyncKeyState(VK_F1) & 0x8000))
	{
		Sleep(100);
	}

#ifdef DEBUG
	fclose(fp);
	FreeConsole();
#endif


	FreeLibraryAndExitThread(hmod, 0);
	return TRUE;
}



//void uninject()
//{
//	UnhookVEH();
//	FreeConsole();
//	VirtualFree(hModule, 0, MEM_RELEASE);
//}

HMODULE hModule;
BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		hModule = hMod;
#ifdef MANUALMAPPER //if we are using manual mapper, we need to call the main thread manually. Not fully implemented cause it depends on the injector
		MainThread(hMod);
#else
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hMod, 0, nullptr);
#endif

		

		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

