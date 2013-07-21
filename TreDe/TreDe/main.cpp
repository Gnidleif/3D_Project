#pragma once
#include "D3D11App.h"
#include "SettingsClass.h"
#include "TextClass.h"
#include "TextureClass.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "LoaderClass.h"
#include "ModelClass.h"

#include "Game.h"
#include "SkyBox.h"

#include <iostream>
using namespace std;
// The main class, used to handle the "game-loop" and initializing/shutdown of various static classes

enum DrawMode
{
	Light,
	Solid,
	Wire
};

class Main : public D3D11App
{
public:
	Main(HINSTANCE hInst);
	~Main();
	bool Initialize();
	void Update(float dt);
	void Draw();
	void OnResize();

private:
	void Input();

private:
	float mDrawSwitchCD;
	UINT mCurrDraw;
	BYTE* mInput;
	Game* mGame;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prevInst, LPSTR cmdLine, int cmdShow)
{
#if defined (_DEBUG) | (DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(322);
	if(AllocConsole())
	{
		freopen("CONOUT$", "wt", stdout);
		SetConsoleTitle("Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);  
	}
#endif
	Main app(hInst);

	if( FAILED(app.Initialize()))
	{
		cout << "Failed to initialize app!" << endl;
		return E_FAIL;
	}
	return app.Run();
}

Main::Main(HINSTANCE hInst)
	: 
	D3D11App(hInst),
	mGame(new Game()),
	//mInput(nullptr),
	mCurrDraw(Light),
	mDrawSwitchCD(0.0f)
{
	mMainWndCaption = "3D2 AwesomesauceMegaSuperTruperBanana Project";
}
// Responsible for shutting down all the singletons, seems easier that way
Main::~Main()
{
	Settings->Shutdown();
	Text->Shutdown();
	Texture->Shutdown();
	Effects::Shutdown();
	InputLayouts::Shutdown();
	Loader->Shutdown();
	Model->Shutdown();

	SafeDelete(mGame);
}

bool Main::Initialize()
{
	// Initialize window settings first, since later classes will be using these
	if(!Settings->Initialize())
	{
		cout << "Failed to initialize SettingsClass." << endl;
		return false;
	}

	// D3D11App initialized
	if(!D3D11App::Initialize())
	{
		cout << "Failed to initialize app." << endl;
		return false;
	}

	Text->Initialize(mDirect3D->GetDevice(), mDirect3D->GetDevCon());
	Texture->Initialize(mDirect3D->GetDevice());
	Effects::Initialize(mDirect3D->GetDevice());
	InputLayouts::Initialize(mDirect3D->GetDevice());
	Loader->Initialize(mDirect3D->GetDevice());
	Model->Initialize(mDirect3D->GetDevice());

	mGame->Initialize(mDirect3D->GetDevice());

	// Last part of the initialize of main
	D3D11App::ShowWindow();
	OnResize();
	return S_OK;
}

void Main::Update(float dt)
{
	mDrawSwitchCD += dt;

	this->Input();

	Text->Update(dt);
	mGame->Update(dt);
}

void Main::Draw()
{
	mDirect3D->GetDevCon()->ClearRenderTargetView(mDirect3D->GetRTView(), reinterpret_cast<const float*>(&Colors::Black));
	mDirect3D->GetDevCon()->ClearDepthStencilView(mDirect3D->GetDSView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	switch(mCurrDraw)
	{
	case Solid:
		mGame->SolidDraw(mDirect3D->GetDevCon());
		break;
	case Light:
		mGame->LightDraw(mDirect3D->GetDevCon());
		break;
	case Wire:
		mGame->WireDraw(mDirect3D->GetDevCon());
		break;
	}

	// If the text isn't drawn last, objects in the world might hide it
	Text->Draw();

	mDirect3D->GetSwapChain()->Present(1, 0);
}

void Main::OnResize()
{
	D3D11App::OnResize();
	// Converts the height/width of the resolution to strings and puts them as a constant text to be shown
	string screenX = Text->IntToString(Settings->GetData()->mWidth);
	string screenY = Text->IntToString(Settings->GetData()->mHeight);
	Text->AddConstantText("ScreenDims", "Resolution: " + screenX + "x" + screenY, 20.0f, 110.0f, 20.0f, TextColors::Red);

	mGame->GetPlayerCam()->SetLens(0.25f*MathHelper::pi, D3D11App::AspectRatio(), 1.0f, MathHelper::infinity);
	mGame->GetPlayerCam()->ComputeFrustum();
}

void Main::Input()
{
	// Not really optimal to put this here, but whatever
	if(mInput == D3D11App::mDirectInput->GetKeyboardState())
	{
		//BYTE* lpInput = D3D11App::mDirectInput->GetKeyboardState();
		if(mInput[DIK_1] && 0x80)
		{
			Settings->SetResolution(800, 600);
			D3D11App::SwitchResolution();
		}
		else if(mInput[DIK_2] && 0x80)
		{
			Settings->SetResolution(1024, 768);
			D3D11App::SwitchResolution();
		}
		else if(mInput[DIK_3] && 0x80)
		{
			Settings->SetResolution(1600, 900);
			D3D11App::SwitchResolution();
		}
		else if(mInput[DIK_NUMPAD0] && 0x80 && mDrawSwitchCD > 1.0f)
		{
			mDrawSwitchCD = 0.0f;
			mCurrDraw = Light;
		}
		else if(mInput[DIK_NUMPAD1] && 0x80 && mDrawSwitchCD > 1.0f)
		{
			mDrawSwitchCD = 0.0f;
			mCurrDraw = Solid;
		}
		else if(mInput[DIK_NUMPAD2] && 0x80 && mDrawSwitchCD > 1.0f)
		{
			mDrawSwitchCD = 0.0f;
			mCurrDraw = Wire;
		}
		else if(mInput[DIK_ESCAPE] && 0x80)
		{
			SendMessage(mhMainWnd, WM_DESTROY, 0, 0);
		}
		// If any button is pressed that isn't in any of the above buttons,
		// the input-stuff for the player is checked through the game-object instead
		else
		{
			mGame->ControlPlayer(D3D11App::mDirectInput);
		}
	}
	// Kind of an ugly solution to a bug that changed the resolution on startup for some reason
	mInput = D3D11App::mDirectInput->GetKeyboardState();
}