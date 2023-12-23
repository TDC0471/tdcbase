#pragma once
#include <Windows.h>

class VEHHook
{
	DWORD oldProc;
	void* SwapChainVEHHandler;
	void* funcAddress;

public:
	VEHHook() = default;
	VEHHook(PVECTORED_EXCEPTION_HANDLER handler, void* funcAddress);

	void RemoveHook();

	//delete copy constructor and assignment operator
	VEHHook(const VEHHook&) = delete;
	VEHHook& operator=(const VEHHook&) = delete;

	//implement move constructor and assignment operator
	VEHHook(VEHHook&&) = default;
	VEHHook& operator=(VEHHook&&) = default;

	~VEHHook();
};