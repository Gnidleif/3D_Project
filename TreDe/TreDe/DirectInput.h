#pragma once
#ifndef DIRECTINPUT_H_
#define DIRECTINPUT_H_

// Pre-processing
#define DIRECTINPUT_VERSION 0x0800

// Linking
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <dinput.h>
#include "SettingsClass.h"

class DirectInput
{
public:
	DirectInput(void);
	DirectInput(const DirectInput& other) {}
	~DirectInput(void) {}

	bool Initialize(HINSTANCE* appInstance, HWND* mainWindow,
		int* cWidth, int* cHeight);
	void Shutdown();
	void Update();

	bool MouseHasMoved();

	// Get states
	BYTE* GetKeyboardState() { return this->mKeyboardState; }
	DIMOUSESTATE GetMouseState() { return this->mMouseCurState; }

	void SetLastMouseState(DIMOUSESTATE* mouseState);

	// Get devices
	IDirectInputDevice8* GetMouseDevice() const { return this->mDiMouse; }
	IDirectInputDevice8* GetKeyboardDevice() const { return this->mDiKeyboard; }

	// Mouse position
	void GetMousePosition(int& x, int& y);

private:
	// Update inputs
	void GetKeyboardInput();
	void GetMouseInput();

	// COM Interfaces
	IDirectInputDevice8* mDiMouse;
	IDirectInputDevice8* mDiKeyboard;

	// Mouse state
	DIMOUSESTATE mMouseLastState;

	DIMOUSESTATE mMouseCurState;

	// Keyboard input
	LPDIRECTINPUT8 mDirectInput;

	// Keyboard states
	BYTE mKeyboardState[256];

	int mMouseX, mMouseY;

	int* mcWidth;
	int* mcHeight;
};

#endif