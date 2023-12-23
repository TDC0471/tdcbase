#include "DxHooks.h"

LRESULT DxHooks::hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

void DxHooks::hkCopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource)
{
	using f_CopyResource = void(__stdcall*)(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource);
	static f_CopyResource oCopyResource{ (f_CopyResource)hooker->getSwapChainVmtFunction(FunctionScrapper::indexOfVirtual(&ID3D11DeviceContext::CopyResource)) }; //initalized at first call
	oCopyResource(pContext, pDstResource, renderer->CopyResource(pContext, pDstResource, pSrcResource));
}

HRESULT DxHooks::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	using f_Present = HRESULT(__stdcall*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static f_Present oPresent = (f_Present)hooker->getSwapChainVmtFunction(FunctionScrapper::indexOfVirtual(&IDXGISwapChain::Present)); //initalized at first call

	if (static bool ran = false; !ran && renderer->GetWindow() != nullptr) //ug ugly
	{
		ran = true;
		oWndProc = (WNDPROC)SetWindowLongPtr(renderer->GetWindow(), GWLP_WNDPROC, (LONG_PTR)hkWndProc);
	}
		

	renderer->Present(pSwapChain, SyncInterval, Flags);



	return oPresent(pSwapChain, SyncInterval, Flags);
}

IDXGISwapChain* DxHooks::dummySwapchain()
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

	//print address of swapchain
	c::printf("Swapchain address: %p\n", swapChain);

	return swapChain;
}
