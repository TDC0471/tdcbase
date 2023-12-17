//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>
#include <stdio.h>
#include <dxgi.h>
#include <d3d11.h>
#include "imguiincl.h"
#include "memory.h"
#include <typeinfo>
#include <intrin.h>
#include "VEHHook.h"
#include "console.h"
#include "ShadowVmtHook.h"

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

using f_Present = HRESULT(__stdcall*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
using f_GetBuffer = HRESULT(__stdcall*)(IDXGISwapChain* pSwapChain, UINT Buffer, REFIID riid, void** ppSurface);
using f_CopyResource = void(__stdcall*)(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource);

IDXGISwapChain* dummySwapchain()
{
	HWND window = GetForegroundWindow();

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

	DXGI_RATIONAL refreshRate;
	refreshRate.Numerator = 60;
	refreshRate.Denominator = 1;

	DXGI_MODE_DESC bufferDesc;
	bufferDesc.Width = 100;
	bufferDesc.Height = 100;
	bufferDesc.RefreshRate = refreshRate;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &context)))
	{
		SAFE_RELEASE(device);
		SAFE_RELEASE(context);
		return nullptr;
	}

	SAFE_RELEASE(device);
	SAFE_RELEASE(context);

	return swapChain;
}

template<typename T>
void* funcToVoid(T func)
{
	union
	{
		T fn;
		void* ptr;
	} u;
	u.fn = func;
	return u.ptr;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//imgui wndproc
WNDPROC oWndProc;
LRESULT hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

//void** _ppSurface;
//f_GetBuffer oGetBuffer;
//HRESULT hkGetBuffer(IDXGISwapChain* pSwapChain, UINT Buffer, REFIID riid, void** ppSurface)
//{
//
//	_ppSurface = ppSurface;
//	//c::printf("GetBuffer called\n");
//	return oGetBuffer(pSwapChain, Buffer, riid, ppSurface);
//}

ID3D11Texture2D* pCapturedFrame{};
f_CopyResource oCopyResource;
bool streamproof = false; 
void hkCopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource)
{
	if(!streamproof)
		return oCopyResource(pContext, pDstResource, pSrcResource);

	return oCopyResource(pContext, pDstResource, pCapturedFrame);

	//if (_ppSurface && *_ppSurface == pSrcResource)
	//{
	//	c::printf("Surface is being copied\n");
		//return oCopyResource(pContext, pDstResource, pCapturedFrame);
	//}

	//return oCopyResource(pContext, pDstResource, pSrcResource);
}

f_Present oPresent;

bool copybb(IDXGISwapChain* pSwapChain, ID3D11DeviceContext* pContext)
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer))))
	{
		oCopyResource(pContext, pCapturedFrame, pBackBuffer);
		pBackBuffer->Release(); // Release the back buffer reference
		return true;
	}
	return false;
}

HRESULT hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static HWND window{ NULL };
	static ID3D11Device* pDevice{ NULL };
	static ID3D11DeviceContext* pContext{ NULL };
	static ID3D11RenderTargetView* mainRenderTargetView;
	static bool init = false;
	static UINT streamfps = 0;

	if (!init) 
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice))))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hkWndProc)));

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(pDevice, pContext);

			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = sd.BufferDesc.Width;
			desc.Height = sd.BufferDesc.Height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = sd.BufferDesc.Format;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			pDevice->CreateTexture2D(&desc, nullptr, &pCapturedFrame);

			copybb(pSwapChain, pContext);

			init = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	static ULONGLONG lasttime = GetTickCount64();
	if (streamproof && GetTickCount64() - lasttime >= streamfps)
	{
		lasttime = GetTickCount64();
		copybb(pSwapChain, pContext);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin("ImGui Window");
	ImGui::Checkbox("Streamproof", &streamproof);
	
	static const int min = 0, max = 1000;
	ImGui::SliderScalar("Streamproof Updatetime (Mills)", ImGuiDataType_U32, &streamfps, &min, &max);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Console");
	ImGui::Text(c::console.c_str());
	ImGui::End();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

IDXGISwapChain* swapChain;
VEHHook* SwapChainHook;
ShadowVmtHook swapChainVmtHook;
ShadowVmtHook deviceContextVmtHook;
LONG WINAPI SwapChainVEHHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	c::printf("Caught exception\n");

	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //We will catch PAGE_GUARD Violation
	{
		c::printf("Caught PAGE_GUARD Violation\n");
		if (pExceptionInfo->ContextRecord->Rip == (uintptr_t)oPresent)
		{
			c::printf("Caught PAGE_GUARD Violation at address\n");

			//rcx is the first argument of the function, which is the swapchain
			//we want to replace the swapchain's vtable with our own

			swapChain = reinterpret_cast<IDXGISwapChain*>(pExceptionInfo->ContextRecord->Rcx);
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

					void** vtable = *reinterpret_cast<void***>(pContext);
					oCopyResource = reinterpret_cast<f_CopyResource>(vtable[47]);
					swapChainVmtHook = { swapChain, 18, 1, VirtualMethod{ &hkPresent, 8 } };
					deviceContextVmtHook = { pContext, 108, 1, VirtualMethod{ &hkCopyResource, 47 } };
					delete SwapChainHook;

				}
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			
		}
		pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed. In short, we come right back into this exception handler 1 instruction later
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) //We will also catch STATUS_SINGLE_STEP, meaning we just had a PAGE_GUARD violation
	{
		DWORD dwOld;
		VirtualProtect((LPVOID)oPresent, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag because everytime it is triggered, it get removes

		return EXCEPTION_CONTINUE_EXECUTION; //Continue the next instruction
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

DWORD WINAPI MainThread(HMODULE hmod)
{

	c::printf("Hello from DLL!\n");

	IDXGISwapChain* pSwapChain = dummySwapchain();


	void** vtable = *reinterpret_cast<void***>(pSwapChain);
	
	oPresent = reinterpret_cast<f_Present>(vtable[8]);

	c::printf("Present: 0x%p\n", oPresent);

	SwapChainHook = new VEHHook{ SwapChainVEHHandler, oPresent };

	////while f1 is not pressed
	//while (!(GetAsyncKeyState(VK_F1) & 0x8000))
	//{
	//	Sleep(100);
	//}

	//VirtualFree(pSwapChain, 0, MEM_RELEASE);

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
#ifdef MANUALMAPPER
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

