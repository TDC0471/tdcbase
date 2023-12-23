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

class Renderer
{

	ID3D11Texture2D* pCapturedFrame{};
	ID3D11Hooker * const Hooker{};

	HWND window{};
	ID3D11Device* pDevice{};
	ID3D11DeviceContext* pContext{};
	ID3D11RenderTargetView* mainRenderTargetView{};

	bool streamproof = true;

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

	void Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	void WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void CopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource*& pSrcResource, void* returnAddress = _ReturnAddress()); //return value is the source resource that will be copied to the destination resource HATE THIS

private:

	bool copybb(IDXGISwapChain* pSwapChain, ID3D11DeviceContext* pContext);

};