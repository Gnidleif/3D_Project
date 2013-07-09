#include "Player.h"

Player::Player(std::string name, XMFLOAT3 pos)
	:
	mName(name),
	mSpeed(1.0f),
	mPos(pos),
	mCamera(new Camera())
{
	mCamera->SetPosition(mPos);
}

Player::~Player(void)
{
	SafeDelete(mCamera);
}

void Player::Update(float dt)
{
	mCamera->UpdateViewMatrix();
}

void Player::Control(DirectInput* di)
{
	BYTE* lpInput = nullptr;
	if(di->MouseHasMoved())
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(di->GetMouseState().lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(di->GetMouseState().lY));
		float yaw = mCamera->GetYaw() + dx;
		float pitch = mCamera->GetPitch() + dy;

		mCamera->SetYaw(yaw);
		mCamera->SetPitch(pitch);
	}
	if(lpInput = di->GetKeyboardState())
	{
		XMVECTOR look = XMVector3Normalize(mCamera->GetLookXM());
		XMVECTOR right = XMVector3Normalize(mCamera->GetRightXM());
		XMVECTOR movement = XMLoadFloat3(&mPos);
		if(lpInput[DIK_W] && 0x80)
		{
			movement += look * mSpeed;
		}
		if(lpInput[DIK_A] && 0x80)
		{
			movement -= right * mSpeed;
		}
		if(lpInput[DIK_S] && 0x80)
		{
			movement -= look * mSpeed;
		}
		if(lpInput[DIK_D] && 0x80)
		{
			movement += right * mSpeed;
		}
		XMStoreFloat3(&mPos, movement);
		mCamera->SetPosition(mPos);
	}
}