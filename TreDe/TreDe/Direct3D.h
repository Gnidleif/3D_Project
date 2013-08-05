#ifndef DIRECT3D_H
#define DIRECT3D_H
#include "d3dUtilities.h"

class Direct3D
{
public: // Public functions
	Direct3D(void);
	~Direct3D(void) {}

	bool Initialize(HWND* hWnd);
	void OnResize();
	void Shutdown();

public: // Get functions
	ID3D11Device* GetDevice() const { return this->mDevice; }
	ID3D11DeviceContext* GetDevCon() const { return this->mDevCon; }
	ID3D11RenderTargetView* GetRTView() const { return this->mRTView; }
	ID3D11DepthStencilView* GetDSView() const { return this->mDSView; }
	D3D11_VIEWPORT GetScreenViewport() const { return this->mScreenViewport; }
	IDXGISwapChain* GetSwapChain() const { return this->mSwapChain; }

private: // Privates! Don't touch :C
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDevCon;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDSBuffer;
	ID3D11RenderTargetView* mRTView;
	ID3D11DepthStencilView* mDSView;
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE mDriverType;

	bool mEnable4xMsaa;
	UINT m4xMSAAQuality;

	UINT* mcWidth;
	UINT* mcHeight;
};
#endif