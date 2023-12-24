#include "VEHD3D11Hooker.h"
#include "console.h"
#include "FunctionScrapper.h"
#include "DxHooks.h"


//define the static variables
ShadowVmtHook* VEHD3D11Hooker::swapChainHook;
ShadowVmtHook* VEHD3D11Hooker::deviceContextHook;
VEHHook* VEHD3D11Hooker::vehHook;
void* VEHD3D11Hooker::oPresent;

VEHD3D11Hooker::~VEHD3D11Hooker()
{
	c::printf("VEHD3D11Hooker dtor\n");
	delete swapChainHook;
	delete deviceContextHook;
	delete vehHook;
}

void VEHD3D11Hooker::init()
{
	//oPresent = FunctionScrapper::addressOfVirtual(DxHooks::dummySwapchain(), &IDXGISwapChain::Present);
	oPresent = reinterpret_cast<void***>(DxHooks::dummySwapchain())[0][8];

	c::printf("VeH D3D11 Hooker constructor\n");

	auto SwapChainVEHHandler = [](PEXCEPTION_POINTERS pExceptionInfo) -> LONG
	{

		if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //We will catch PAGE_GUARD Violation
		{
			if (pExceptionInfo->ContextRecord->Rip == reinterpret_cast<DWORD_PTR>(oPresent))
			{

				//rcx is the first argument of the function, which is the swapchain
				//we want to replace the swapchain's vtable with our own

				auto swapChain = reinterpret_cast<IDXGISwapChain*>(pExceptionInfo->ContextRecord->Rcx);
				if (swapChain) {
					c::printf("Swapchain vaild\n");
					//hkPresent 9
					//hkGetBuffer 10


					//get pDeviceContext
					if (ID3D11Device* pDevice{}; FAILED(swapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice))))
					{
						c::printf("Failed to get device\n");
					}
					else
					{
						ID3D11DeviceContext* pContext{};
						pDevice->GetImmediateContext(&pContext);

						swapChainHook = new ShadowVmtHook{ swapChain, 18, { {&DxHooks::hkPresent, FunctionScrapper::indexOfVirtual(&IDXGISwapChain::Present) }, {&DxHooks::hkResizeBuffers, FunctionScrapper::indexOfVirtual(&IDXGISwapChain::ResizeBuffers) }}}; //use functionScrapper to get the index of the function
						deviceContextHook = new ShadowVmtHook{ pContext, 108, { { &DxHooks::hkCopyResource, FunctionScrapper::indexOfVirtual(&ID3D11DeviceContext::CopyResource) }}};
						delete vehHook;
						return EXCEPTION_CONTINUE_EXECUTION;
					}

				}

			}
			pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed. In short, we come right back into this exception handler 1 instruction later
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) //We will also catch STATUS_SINGLE_STEP, meaning we just had a PAGE_GUARD violation
		{
			DWORD dwOld;
			VirtualProtect(reinterpret_cast<LPVOID>(oPresent), 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag because everytime it is triggered, it get removes

			return EXCEPTION_CONTINUE_EXECUTION; //Continue the next instruction
		}

		return EXCEPTION_CONTINUE_SEARCH;
	};

	vehHook = new VEHHook{ SwapChainVEHHandler, oPresent };
}

void* VEHD3D11Hooker::getSwapChainVmtFunction(int index)
{
	return swapChainHook->getOriginalFunction(index);
}

void* VEHD3D11Hooker::getDeviceContextVmtFunction(int index)
{
	return deviceContextHook->getOriginalFunction(index);
}
