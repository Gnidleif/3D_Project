#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include "d3dUtilities.h"
#include "xnacollision.h"

class Camera
{
public:
	Camera();
	~Camera();

	void SetYawPitchRoll(XMFLOAT3 YawPitchRoll);

	// Get frustum (lens) properties, not defined here because of them belonging to a #pragma region
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	// Set frustum (lens)
	void SetLens(float fovY, float aspect, float zn, float zf);
	void UpdateViewMatrix();
	void ComputeFrustum();

public: // Simple set functions
	void SetYaw(float yaw) { this->mYaw = yaw; }
	void SetPitch(float pitch) { this->mPitch = pitch; }
	void SetRoll(float roll) { this->mRoll = roll; }

	void SetPosition(float x, float y, float z) { this->mPosition = XMFLOAT3(x, y, z); }
	void SetPosition(const XMFLOAT3& v) { this->mPosition = v; }
	
public: // Simple get functions
	XMVECTOR GetYawPitchRollXM() const { return XMLoadFloat3(&XMFLOAT3(mYaw, mPitch, mRoll)); }
	XMFLOAT3 GetYawPitchRoll() const { return XMFLOAT3(mYaw, mPitch, mRoll); }

	XMVECTOR GetPositionXM() const { return XMLoadFloat3(&mPosition); }
	XMFLOAT3 GetPosition() const { return this->mPosition; }

	float GetYaw() const { return this->mYaw; }
	float GetPitch() const { return this->mPitch; }
	float GetRoll() const { return this->mRoll; }

	// Get functions
	XMVECTOR GetRightXM() const { return XMLoadFloat3(&this->mRight); }
	XMFLOAT3 GetRight() const { return this->mRight; }
	XMVECTOR GetUpXM() const { return XMLoadFloat3(&this->mUp); }
	XMFLOAT3 GetUp() const { return this->mUp; }
	XMVECTOR GetLookXM() const { return XMLoadFloat3(&this->mLook); }
	XMFLOAT3 GetLook() const { return this->mLook; }

	// Get matrices
	XMMATRIX GetViewMatrix() const { return XMLoadFloat4x4(&this->mView); }
	XMMATRIX GetProjMatrix() const { return XMLoadFloat4x4(&this->mProj); }
	XMMATRIX GetViewProjMatrix() const { return XMMatrixMultiply(this->GetViewMatrix(), this->GetProjMatrix()); }

	// Get near and far plane dimensions in view space coordinates
	float GetNearWindowWidth() const { return this->mAspect * this->mNearWindowHeight; }
	float GetNearWindowHeight() const { return this->mNearWindowHeight; }
	float GetFarWindowWidth() const { return this->mAspect * this->mFarWindowHeight; }
	float GetFarWindowHeight() const { return this->mFarWindowHeight; }
	
	XNA::Frustum GetFrustum() const { return this->mFrustum; }

private: // Private variables
	// Coordinate system relative to world space
	XMFLOAT3 mPosition; // view space origin
	XMFLOAT3 mRight; // view space x-axis
	XMFLOAT3 mUp; // view space y-axis
	XMFLOAT3 mLook; // view space z-axis

	// Yaw, pitch, roll
	float mYaw;
	float mPitch;
	float mRoll;

	// Matrix cache
	XMFLOAT4X4 mView; // View matrix
	XMFLOAT4X4 mProj; // Projection matrix

	// Frustum cache
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	XNA::Frustum mFrustum;
};
#endif