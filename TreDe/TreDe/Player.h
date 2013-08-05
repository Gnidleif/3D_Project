#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "Camera.h"
#include "DirectInput.h"

class Player
{
public:
	Player(std::string name, XMFLOAT3 pos);
	~Player(void);
	void Update(float dt);
	void Control(DirectInput* di);

public:
	std::string GetName() const { return this->mName; }
	Camera* GetCamera() { return this->mCamera; }
	XMFLOAT3 GetPosition() const { return this->mPos; }

private:
	void SetPosition(XMFLOAT3 pos) { this->mPos = pos; }

private:
	std::string mName;
	float mSpeed;
	Camera* mCamera;
	XMFLOAT3 mPos;
};
#endif

