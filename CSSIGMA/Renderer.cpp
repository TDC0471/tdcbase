#include "Renderer.h"
#include "memory.h"
#include "utils.h"

void Renderer::Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	//c::printf("Present called\n");
	static bool init = false;
	if (!init)
	{
		if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice)))) return;

		pDevice->GetImmediateContext(&pContext);
		DXGI_SWAP_CHAIN_DESC sd;
		if (FAILED(pSwapChain->GetDesc(&sd))) return;
		window = sd.OutputWindow;
		ID3D11Texture2D* pBackBuffer;
		if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) return;
		if (FAILED(pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView))) return;
		pBackBuffer->Release();

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


	static UINT streamfps = 0;


	static ULONGLONG lasttime = GetTickCount64();
	if (streamproof && GetTickCount64() - lasttime >= streamfps)
	{
		lasttime = GetTickCount64();
		copybb(pSwapChain, pContext);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//call callbacks

	ImGui::Begin("ImGui Window");
	ImGui::Checkbox("Streamproof", &streamproof);

	static const int min = 0, max = 1000;
	ImGui::SliderScalar("Streamproof Updatetime (Mills)", ImGuiDataType_U32, &streamfps, &min, &max);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Console");
	ImGui::Text(c::get()->c_str());
	ImGui::End();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
}

void Renderer::CopyResource(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, ID3D11Resource*& pSrcResource, void* returnAddress)
{
	static void* lastinvaildaddress = nullptr;
	//virtual query if Return address is in a disallowed module (For now just Discord)

	if (streamproof) 
	{
		//if (lastinvaildaddress != returnAddress) 
		//{
		//	if (cmpncend(memory::GetModuleName(returnAddress).get(), "DiscordHook64.dll")) //could make a function for this "isAddressInModule"
		//	{
		//		return;
		//	}
		//	lastinvaildaddress = returnAddress;
		//}
		pSrcResource = pCapturedFrame;
	}
	

}

bool Renderer::copybb(IDXGISwapChain* pSwapChain, ID3D11DeviceContext* pContext)
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer))))
	{
		using f_CopyResource = void(*)(ID3D11DeviceContext*, ID3D11Resource*, ID3D11Resource*);

		reinterpret_cast<f_CopyResource>(Hooker->getDeviceContextVmtFunction(FunctionScrapper::indexOfVirtual(&ID3D11DeviceContext::CopyResource)))
			(pContext, pCapturedFrame, pBackBuffer);
		pBackBuffer->Release(); // Release the back buffer reference
		return true;
	}
	return false;
}
