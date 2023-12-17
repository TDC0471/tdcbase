#pragma once
#include <Windows.h>

class VEHHook
{
	DWORD oldProc;
	void* SwapChainVEHHandler;
	void* funcAddress;

public:
	VEHHook(PVECTORED_EXCEPTION_HANDLER handler, void* funcAddress);

	~VEHHook();
};