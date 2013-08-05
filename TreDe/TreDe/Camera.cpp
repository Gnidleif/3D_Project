#include "Camera.h"

Camera::Camera()
	: 
	mPosition	(0.0f, 0.0f, 0.0f), 
	mRight		(1.0f, 0.0f, 0.0f),
	mUp			(0.0f, 1.0f, 0.0f),
	mLook		(0.0f, 0.0f, 1.0f),

	mYaw(0.0f), mPitch(0.0f), mRoll(0.0f)
{
	XMStoreFloat4x4(&mView, XMMatrixIdentity());
	XMStoreFloat4x4(&mProj, XMMatrixIdentity());
}

Camera::~Camera()
{
}

void Camera::SetYawPitchRoll(XMFLOAT3 YawPitchRoll)
{
	this->mYaw = YawPitchRoll.x;
	this->mPitch = YawPitchRoll.y;
	this->mRoll = YawPitchRoll.z;
}

#pragma region Frustum
float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetAspect() const
{
	return mAspect;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const // Save here
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

void Camera::SetLens( float fovY, float aspect, float zn, float zf )
{
	// Cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}
#pragma endregion Frustum

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);

	XMVECTOR Up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	XMVECTOR Look = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
	XMVECTOR Right = XMLoadFloat3(&XMFLOAT3(1, 0, 0));

	XMMATRIX yawMatrix = XMMatrixRotationAxis(Up, mYaw);
	Look = XMVector3TransformCoord(Look, yawMatrix);
	Right = XMVector3TransformCoord(Right, yawMatrix);

	XMMATRIX pitchMatrix = XMMatrixRotationAxis(Right, mPitch);
	Look = XMVector3TransformCoord(Look, pitchMatrix);
	Up = XMVector3TransformCoord(Up, pitchMatrix);

	XMMATRIX rollMatrix = XMMatrixRotationAxis(Look, mRoll);
	Right = XMVector3TransformCoord(Right, rollMatrix);
	Up = XMVector3TransformCoord(Up, rollMatrix);

	mView(0,0) = XMVectorGetX(Right); mView(0,1) = XMVectorGetX(Up); mView(0,2) = XMVectorGetX(Look);
	mView(1,0) = XMVectorGetY(Right); mView(1,1) = XMVectorGetY(Up); mView(1,2) = XMVectorGetY(Look);
	mView(2,0) = XMVectorGetZ(Right); mView(2,1) = XMVectorGetZ(Up); mView(2,2) = XMVectorGetZ(Look);

	mView(3,0) = -XMVectorGetX(XMVector3Dot(pos, Right));
	mView(3,1) = -XMVectorGetX(XMVector3Dot(pos, Up));
	mView(3,2) = -XMVectorGetX(XMVector3Dot(pos, Look));
	
	mView(0,3) = 0;
	mView(1,3) = 0;
	mView(2,3) = 0;
	mView(3,3) = 1;

	XMStoreFloat3(&mRight, Right);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);
}

void Camera::ComputeFrustum()
{
	ComputeFrustumFromProjection(&mFrustum, &GetProjMatrix());
}
