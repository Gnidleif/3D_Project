#pragma once
#include "D3D11App.h"
#include "SettingsClass.h"
#include "TextClass.h"
#include "TextureClass.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "LoaderClass.h"
#include "ModelClass.h"
#include "LightHandler.h"
#include "ParticleSystem.h"

#include "Game.h"
#include "SkyBox.h"

#include <iostream>
using namespace std;
// The main class, used to handle the "game-loop" and initializing/shutdown of various static classes

enum DrawMode
{
	Light,
	Solid,
	Wire,
	LightTess,
	SolidTess,
	WireTess,
	Shadow
};

const float mSwitchCD = 1.0f;
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
	void SwitchDraw(UINT drawMode);
	void ShadowDraw();
	void ResetRTV();

private:
	float mCurrCD;
	UINT mCurrDraw;
	BYTE* mInput;
	Game* mGame;
	LightHandler* mLightHandler;
	ParticleSystem* mSun;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prevInst, LPSTR cmdLine, int cmdShow)
{
#if defined (_DEBUG) | (DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(175);
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
	mCurrCD(0.0f),
	mLightHandler(new LightHandler()),
	mSun(new ParticleSystem())
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
	SafeDelete(mLightHandler);
	SafeDelete(mSun);
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
	Texture->Initialize(mDirect3D->GetDevice(), mDirect3D->GetDevCon());
	Effects::Initialize(mDirect3D->GetDevice());
	InputLayouts::Initialize(mDirect3D->GetDevice());
	Loader->Initialize(mDirect3D->GetDevice());
	Model->Initialize(mDirect3D->GetDevice());
	mLightHandler->Initialize(mDirect3D->GetDevice());

	mSun->Initialize(
		mDirect3D->GetDevice(), 
		mDirect3D->GetDevCon(),
		Effects::SunFX,
		"../Data/Textures/sun.dds",
		100);

	mGame->Initialize(mDirect3D->GetDevice());

	// Last part of the initialize of main
	D3D11App::ShowWindow();
	OnResize();
	return S_OK;
}

void Main::Update(float dt)
{
	mCurrCD += dt;

	this->Input();
	mLightHandler->Update(dt);
	Text->Update(dt);
	mGame->Update(dt);
	mSun->Update(dt, D3D11App::mTimer.getTimeElapsedS());
	mSun->SetEmitPos(mLightHandler->GetPoint0Pos());
}

void Main::Draw()
{
	switch(mCurrDraw)
	{
	case Solid:
		this->ResetRTV();
		mLightHandler->ApplyEffects();
		mGame->SolidDraw(mDirect3D->GetDevCon());
		break;
	case Light:
		this->ResetRTV();
		mLightHandler->ApplyEffects();
		mGame->LightDraw(mDirect3D->GetDevCon());
		break;
	case Wire:
		this->ResetRTV();
		mLightHandler->ApplyEffects();
		mGame->WireDraw(mDirect3D->GetDevCon());
		break;
	case LightTess:
		this->ResetRTV();
		mLightHandler->ApplyTessEffects();
		mGame->LightTessDraw(mDirect3D->GetDevCon());
		break;
	case SolidTess:
		this->ResetRTV();
		mLightHandler->ApplyTessEffects();
		mGame->SolidTessDraw(mDirect3D->GetDevCon());
		break;
	case WireTess:
		this->ResetRTV();
		mLightHandler->ApplyTessEffects();
		mGame->WireTessDraw(mDirect3D->GetDevCon());
		break;
	case Shadow:
		this->ShadowDraw();
		mLightHandler->ApplyEffects();
		mGame->ShadowMapDraw(mDirect3D->GetDevCon());
		break;
	}
	mSun->Draw(mGame->GetPlayerCam());

	// If the text isn't drawn last, objects in the world might hide it
	Text->Draw();

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mDirect3D->GetDevCon()->PSGetShaderResources(0, 16, nullSRV);

	mDirect3D->GetDevCon()->RSSetState(0);
	mDirect3D->GetDevCon()->OMSetDepthStencilState(0, 0);

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	mDirect3D->GetDevCon()->OMSetBlendState(0, blendFactor, 0xffffffff);

	mDirect3D->GetSwapChain()->Present(0, 0);
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
		else if(mInput[DIK_NUMPAD0] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Solidframe with lighting", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(Light);
		}
		else if(mInput[DIK_NUMPAD1] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Solidframe", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(Solid);
		}
		else if(mInput[DIK_NUMPAD2] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Wireframe", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(Wire);
		}
		else if(mInput[DIK_NUMPAD3] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Solidframe with lighting and tessellation", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(LightTess);
		}
		else if(mInput[DIK_NUMPAD4] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Solidframe with tessellation", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(SolidTess);
		}
		else if(mInput[DIK_NUMPAD5] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Wireframe with tessellation", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(WireTess);
		}
		else if(mInput[DIK_NUMPAD6] && 0x80 && mCurrCD > mSwitchCD)
		{
			Text->AddTimedText("DrawType", "Shadowmapping", 20.0f, 200.0f, 20.0f, TextColors::White, 2.0f);
			this->SwitchDraw(Shadow);
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

void Main::SwitchDraw(UINT drawMode)
{
	this->mCurrCD = 0.0f;
	this->mCurrDraw = drawMode;
}

void Main::ShadowDraw()
{
	mLightHandler->Draw(mGame->GetPlatforms(), mGame->GetTerrain(), mDirect3D->GetDevCon(), mGame->GetPlayerCam());

	mDirect3D->GetDevCon()->RSSetState(0);
	this->ResetRTV();
}

void Main::ResetRTV()
{
	ID3D11RenderTargetView* renderTargets[1] = {mDirect3D->GetRTView()};
	mDirect3D->GetDevCon()->OMSetRenderTargets(1, renderTargets, mDirect3D->GetDSView());

	mDirect3D->GetDevCon()->ClearRenderTargetView(mDirect3D->GetRTView(), reinterpret_cast<const float*>(&Colors::Black));
	mDirect3D->GetDevCon()->ClearDepthStencilView(mDirect3D->GetDSView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	mDirect3D->GetDevCon()->RSSetViewports(1, &mDirect3D->GetScreenViewport());
}