#include "D3D11App.h"
#include "TextClass.h"
#include <WindowsX.h>

namespace
{
	// Only used for forwarding Windows messages from a global window procedure
	// to our member specific window procedure
	D3D11App* gd3d11App = 0;
}

D3D11App::D3D11App(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption("Direct3D Application"),
	mcWidth(0),
	mcHeight(0),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	mIsFullscreen(false),
	mDirectInput(nullptr)
{
	// Bind pointer to this object
	gd3d11App = this;
}

D3D11App::~D3D11App(void)
{
	if(mDirectInput)
	{
		mDirectInput->Shutdown();
		SafeDelete(mDirectInput);
	}

	if (mDirect3D)
	{
		mDirect3D->Shutdown();
		SafeDelete(mDirect3D);
	}
}

//================================================================
// Initialize application
//================================================================
bool D3D11App::Initialize()
{
	// Have to create Direct3D object before initializing
	// window, basically to initialize d3dDevice to 0
	mDirect3D = new Direct3D();

	if (!mDirect3D)
		return false;

	SettingsData lSettings = *Settings->GetData();
	mcWidth = (int)lSettings.mWidth;
	mcHeight = (int)lSettings.mHeight;
	mIsFullscreen = lSettings.mIsFullscreen;

	//-------------------------------------------
	// Init Win32 window
	//-------------------------------------------
	if (!InitMainWindow())
		return false;

	//-------------------------------------------
	// Init Direct3D
	//-------------------------------------------
	if (!mDirect3D->Initialize(&mhMainWnd))
		return false;

	//-------------------------------------------
	// Init Direct Input
	//-------------------------------------------

	mDirectInput = new DirectInput();
	if(!mDirectInput)
		return false;

	if(!mDirectInput->Initialize(&mhAppInst, &mhMainWnd, &mcWidth, &mcHeight))
		return false;

	return true;
}

//================================================================
// Application loop
//================================================================
int D3D11App::Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	mTimer.reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages, process them
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		// Else run the game loop
		else
		{
			mTimer.tick();

			if (!mAppPaused)
			{
				Update(mTimer.getDeltaTime());
				mDirectInput->Update();
				CalculateFrameStats();
				Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

//=========================================================================
// Win32 messages
//=========================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward msg to this object's MsgProc. In case where we get a msg
	// before "CreateWindow" has finished. (That is, before mhMainWnd is valid)
	return gd3d11App->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT D3D11App::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// Window being destroyed
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	// Window activated or deactivated (in focus or not)
	case WM_ACTIVATE:
		if(LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.start();
		}
		return 0;

	// Window is resized
	case WM_SIZE:
		// Get the new client area dimensions
		mcWidth = LOWORD(lParam);
		mcHeight = HIWORD(lParam);

		if (mDirect3D->GetDevice())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}

			else if(wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}

			else if(wParam == SIZE_RESTORED)
			{

				// If we are restoring from minimized state
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// If we are restoring from maximized state
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if(mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	// If the user grabs the resize bars
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing  = true;
		mTimer.stop();
		return 0;

	// If the user releases the resize bars
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing  = false;
		mTimer.start();
		OnResize();
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_SYSCOMMAND:
		// ALT + ENTER
		if (SC_KEYMENU == (wParam & 0xFFF0))
		{
		}
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void D3D11App::SwitchResolution()
{
	SettingsData lSettings = *Settings->GetData();
	mcWidth = lSettings.mWidth;
	mcHeight = lSettings.mHeight;
	BOOL lpIsFullscreen = (BOOL)lSettings.mIsFullscreen;
	mDirect3D->GetSwapChain()->GetFullscreenState(&mIsFullscreen, nullptr);

	// Resize window and set window position at center of screen
	if (!mIsFullscreen)
	{
		SetWindowPos(mhMainWnd, HWND_TOP,
			static_cast<int>((GetSystemMetrics(SM_CXSCREEN)*0.5) - (mcWidth*0.5)),
			static_cast<int>((GetSystemMetrics(SM_CYSCREEN)*0.5) - (mcHeight*0.5)),
			mcWidth, mcHeight, SWP_FRAMECHANGED);
	}

	OnResize();
}

//====================================================================
// Initialize Win32 window
//====================================================================
bool D3D11App::InitMainWindow()
{
	// Register the Window class
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName = 0;
	wc.lpszClassName = "Window";

	if (!RegisterClass(&wc))
	{
		::MessageBox(0, "Failed - Register WNDCLASS", 0, MB_OK);
		return false;
	}

	// Set Window dimensions
	RECT R = { 0, 0, mcWidth, mcHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	// Create Window
	mhMainWnd = ::CreateWindow(
		"Window", 
		mMainWndCaption, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		width, 
		height, 
		0, 
		0, 
		mhAppInst, 
		0);

	if (mhMainWnd == 0)
	{
		::MessageBox(0, "Failed - Creation WNDCLASS", 0, MB_OK);
		return false;
	}

	return true;
}

// Calculate aspect ratio
float D3D11App::AspectRatio() const
{
	return static_cast<float>(mcWidth) / mcHeight;
}

void D3D11App::CalculateFrameStats()
{
	// Computes average FPS, and average time it takes to render one frame
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if ((mTimer.getTimeElapsedS() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount;
		float msPerFrame = 1000.0f / fps;

		Text->AddConstantText("FPS", "FPS: " + Text->FloatToString(fps), 20.0f, 140.0f, 20, TextColors::Red);
		Text->AddConstantText("MSPF", "ms/frame: " + Text->FloatToString(msPerFrame), 20.0f, 170.0f, 20, TextColors::Red);

		// Reset for next average
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

void D3D11App::OnResize()
{
	mDirect3D->OnResize();
}

void D3D11App::SwitchFullscreen()
{
	mAppPaused = true;
	mTimer.stop();
	mIsFullscreen = Settings->GetData()->mIsFullscreen;
	mDirect3D->GetSwapChain()->SetFullscreenState(mIsFullscreen, nullptr);
	mAppPaused = false;
	mTimer.start();
}

void D3D11App::SwitchPaused()
{
	if(mAppPaused)
		mAppPaused = false;
	else
		mAppPaused = true;
}

void D3D11App::ShowWindow()
{
	::ShowWindow(mhMainWnd, SW_SHOW);
	::UpdateWindow(mhMainWnd);
}