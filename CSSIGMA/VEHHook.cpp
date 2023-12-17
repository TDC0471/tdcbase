#include "VEHHook.h"

VEHHook::VEHHook(PVECTORED_EXCEPTION_HANDLER handler, void* funcAddress) : funcAddress(funcAddress)
{
	SwapChainVEHHandler = AddVectoredExceptionHandler(true, handler);
	VirtualProtect(funcAddress, 1, PAGE_EXECUTE | PAGE_GUARD, &oldProc);
}

VEHHook::~VEHHook()
{
	RemoveVectoredExceptionHandler(SwapChainVEHHandler);
	VirtualProtect(funcAddress, 1, oldProc, &oldProc);
}
