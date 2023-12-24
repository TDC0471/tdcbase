#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include "imguiincl.h"
#include "console.h"
#include <vector>
#include "VEHHook.h"
#include "ShadowVmtHook.h"
#include "ID3D11Hooker.h"
#include "FunctionScrapper.h"
#include "ID3D11Hooker.h"
#include <intrin.h>
#include "Event.h"

class Renderer
{

	static Event<> OnPresent; //might be a better way to do this :3
	bool needsToGetElements = true;


	ID3D11Texture2D* pCapturedFrame{};
	ID3D11Hooker * const Hooker{};

	HWND window{};
	ID3D11Device* pDevice{};
	ID3D11DeviceContext* pContext{};
	ID3D11RenderTargetView* mainRenderTargetView{};

	bool streamproof = true; //ewww!!! i would like to make this a "module" but to much overhead!!! (Having like 3 events for one thing is not good)

public:

	//Renderer() = delete;

	Renderer(ID3D11Hooker* hooker) : Hooker(hooker)
	{
		//Hooker = hooker;
	}

	HWND GetWindow() const noexcept
	{
		return window;
	}

	//cause all of these return HRESULTS, and we aren't "aware" of the hooking, we don't need to worry about the return value
	void Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	void WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void CopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource*& pSrcResource, void* returnAddress = _ReturnAddress()); //return value is the source resource that will be copied to the destination resource HATE THIS

	void ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

private:

	bool copybb(IDXGISwapChain* pSwapChain, ID3D11DeviceContext* pContext); //copy back buffer to texture

};