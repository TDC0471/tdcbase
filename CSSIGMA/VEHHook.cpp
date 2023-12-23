#include "VEHHook.h"
#include "console.h"

VEHHook::VEHHook(PVECTORED_EXCEPTION_HANDLER handler, void* funcAddress) : funcAddress(funcAddress)
{
	c::printf("VEHHook ctor\n");
	SwapChainVEHHandler = AddVectoredExceptionHandler(true, handler);
	VirtualProtect(funcAddress, 1, PAGE_EXECUTE | PAGE_GUARD, &oldProc);
}

void VEHHook::RemoveHook()
{
	if (SwapChainVEHHandler) {
		RemoveVectoredExceptionHandler(SwapChainVEHHandler);
		VirtualProtect(funcAddress, 1, oldProc, &oldProc);
		SwapChainVEHHandler = nullptr;
	}
}

VEHHook::~VEHHook()
{
	c::printf("VEHHook dtor\n");
	RemoveHook();
}
