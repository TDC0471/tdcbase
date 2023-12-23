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

class Renderer
{

	ID3D11Texture2D* pCapturedFrame{};
	ID3D11Hooker * const Hooker{};

	HWND window{};
	ID3D11Device* pDevice{};
	ID3D11DeviceContext* pContext{};
	ID3D11RenderTargetView* mainRenderTargetView{};

	bool streamproof = true;

	using f_Callback = void(*)(void);
	std::vector<f_Callback> callbacks;
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

	void AddCallback(f_Callback callback) noexcept;


	void Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	void WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ID3D11Resource* CopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource); //return value is the source resource that will be copied to the destination resource HATE THIS

private:

	bool copybb(IDXGISwapChain* pSwapChain, ID3D11DeviceContext* pContext);

};