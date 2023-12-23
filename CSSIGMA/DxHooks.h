#pragma once
#include "Renderer.h"
#include "ID3D11Hooker.h"
#include <ntstatus.h>
#include "imguiincl.h"

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

namespace DxHooks
{
	//ill accept these heap allocations for now
	inline Renderer* renderer{};
	inline ID3D11Hooker* hooker{};

	//using f_GetBuffer = HRESULT(__stdcall*)(IDXGISwapChain* pSwapChain, UINT Buffer, REFIID riid, void** ppSurface);

	inline WNDPROC oWndProc{};

	//template type must inherit from ID3D11Hooker
	template <typename T>
	void initHooks() {
		static_assert(std::is_base_of<ID3D11Hooker, T>::value, "T must inherit from ID3D11Hooker");

		hooker = new T();
		printf("Hooker created: %p\n", hooker);
		renderer = new Renderer(hooker);
		printf("Hooker created: %p\n", hooker);
		hooker->init();
		printf("Hooker created: %p\n", hooker);
	}

	IDXGISwapChain* dummySwapchain();

	LRESULT hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HRESULT hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	//void** _ppSurface;
//f_GetBuffer oGetBuffer;
//HRESULT hkGetBuffer(IDXGISwapChain* pSwapChain, UINT Buffer, REFIID riid, void** ppSurface)
//{
//
//	_ppSurface = ppSurface;
//	//c::printf("GetBuffer called\n");
//	return oGetBuffer(pSwapChain, Buffer, riid, ppSurface);
//}


	void hkCopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource);

};