#pragma once
#include "ID3D11Hooker.h"
#include "ShadowVmtHook.h"
#include "VEHHook.h"

class VEHD3D11Hooker : public ID3D11Hooker
{
	static ShadowVmtHook* swapChainHook;
	static ShadowVmtHook* deviceContextHook;
	static VEHHook* vehHook;
	static void* oPresent;
public:
	//VEHD3D11Hooker();
	//~VEHD3D11Hooker();
	void* getSwapChainVmtFunction(int index) override;
	void* getDeviceContextVmtFunction(int index) override;
	void init() override;

	~VEHD3D11Hooker();

};
