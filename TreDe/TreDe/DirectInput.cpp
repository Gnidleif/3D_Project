#include "DirectInput.h"

DirectInput::DirectInput(void)
	:
	mDirectInput(nullptr),
	mDiKeyboard(nullptr),
	mDiMouse(nullptr),
	mMouseX(0),
	mMouseY(0),
	mcWidth(nullptr),
	mcHeight(nullptr)
{
}

bool DirectInput::Initialize(HINSTANCE* appInstance, HWND* mainWindow,
	int& _clientWidth, int& _clientHeight)
{
	// Create Direct Input object
	HRESULT hr = DirectInput8Create(	
		*appInstance,					// Handle to app instance
		DIRECTINPUT_VERSION,		// Direct Input version
		IID_IDirectInput8,			// Identifier to DI interface
		(void**)&mDirectInput,		// Pointer to DI object
		nullptr);						// COM aggregation

	//----------------------------------------
	// Create Direct Input devices
	//----------------------------------------
	// Create keyboard device
	hr = mDirectInput->CreateDevice(
		GUID_SysKeyboard,
		&mDiKeyboard,
		nullptr);

	// Create mouse device
	hr = mDirectInput->CreateDevice(
		GUID_SysMouse,
		&mDiMouse,
		nullptr);

	// Set data formats (what kind of input each device has).
	// Also set cooperative levels (how this instance of the device
	// interacts with other instances of the device and the rest of the system

	// Keyboard format
	hr = mDiKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = mDiKeyboard->SetCooperativeLevel(
		*mainWindow,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// Mouse format
	hr = mDiMouse->SetDataFormat(&c_dfDIMouse);
	hr = mDiMouse->SetCooperativeLevel(*mainWindow,
		DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	mcWidth = &_clientWidth;
	mcHeight = &_clientHeight;

	return true;
}

void DirectInput::Shutdown()
{
	if (mDiKeyboard)
	{
		mDiKeyboard->Unacquire();
		mDiKeyboard->Release();
	}

	if (mDiMouse)
	{
		mDiMouse->Unacquire();
		mDiMouse->Release();
	}

	if (mDirectInput)
	{
		mDirectInput->Release();
		mDirectInput = nullptr;
	}
}

void DirectInput::GetKeyboardInput()
{
	mDiKeyboard->Acquire();
	mDiKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);
}

void DirectInput::GetMouseInput()
{
	mDiMouse->Acquire();
	mDiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mMouseCurState);
}

void DirectInput::SetLastMouseState(DIMOUSESTATE* mouseState)
{
	mMouseLastState = *mouseState;
}

void DirectInput::GetMousePosition(int& x, int& y)
{
	x = mMouseX;
	y = mMouseY;
}

void DirectInput::Update()
{
	GetMouseInput();
	GetKeyboardInput();

	mMouseX += mMouseCurState.lX;
	mMouseY += mMouseCurState.lY;

	if (mMouseX < 0)
		mMouseX = 0;

	if (mMouseY < 0)
		mMouseY = 0;

	if (mMouseX > *mcWidth)
		mMouseX = *mcWidth;

	if (mMouseY > *mcHeight)
		mMouseY = *mcHeight;

	//if (mMouseY > GetWindowHeight())

}

bool DirectInput::MouseHasMoved()
{
	if ((mMouseCurState.lX != mMouseLastState.lX)
		|| (mMouseCurState.lY != mMouseLastState.lY))
	{
		mMouseLastState = mMouseCurState;
		return true;
	}

	mMouseLastState = mMouseCurState;
	return false;
}