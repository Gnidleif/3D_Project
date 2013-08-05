#pragma once
#ifndef D3D11APP_H
#define D3D11APP_H

#include "d3dUtilities.h"
#include "GameTimer.h"
#include <string>

#include "DirectInput.h"
#include "Direct3D.h"
#include "SettingsClass.h"

class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	virtual ~D3D11App(void);

	HINSTANCE AppInst() const;
	HWND MainWnd() {return mhMainWnd; }
	float AspectRatio() const;

	UINT GetWindowWidth() const { return this->mcWidth; }
	UINT GetWindowHeight() const { return this->mcHeight; }

	void SwitchFullscreen();
	void SwitchResolution();
	void SwitchPaused();

	//To make the splash screen work we must wait to show the window
	void ShowWindow();

	// Game loop
	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void OnResize(); 
	virtual void Update(float dt)=0;
	virtual void Draw()=0;

	// Convenience overrides for handling mouse input
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:
	bool InitMainWindow();

	void CalculateFrameStats();

	BOOL mIsFullscreen;

	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;

	// Customized values
	int mcWidth;
	int mcHeight;
	LPCSTR mMainWndCaption;

	GameTimer mTimer;

	Direct3D* mDirect3D;
	DirectInput* mDirectInput;
};
#endif

