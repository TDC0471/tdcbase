#pragma once

/*Abstract*/
class ID3D11Hooker
{
public:

	virtual void* getSwapChainVmtFunction(int index) = 0;
	virtual void* getDeviceContextVmtFunction(int index) = 0;
	virtual void init() = 0; //We need to construct the Hooker and pass it to the Renderer before hooks are created
};