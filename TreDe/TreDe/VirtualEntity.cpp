#include "VirtualEntity.h"

VirtualEntity::VirtualEntity()
	:
	mPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	mRotation(0.0f)
{
	XMStoreFloat4x4(&this->mRotMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&this->mModelScale, XMMatrixIdentity());
	XMStoreFloat4x4(&this->mModelRot, XMMatrixIdentity());
	XMStoreFloat4x4(&this->mModelOffset, XMMatrixIdentity());
}

VirtualEntity::~VirtualEntity()
{
}

void VirtualEntity::Initialize(XMFLOAT3 position, float scale)
{
	this->mPosition = position;

	XMStoreFloat4x4(&this->mModelScale, XMMatrixScaling(scale, scale, scale));
	XMStoreFloat4x4(&this->mModelRot, XMMatrixRotationY(mRotation));
	XMStoreFloat4x4(&this->mModelOffset, XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z));
	this->CalcWorld();
}

void VirtualEntity::RotateX(float rot)
{
	XMStoreFloat4x4(&this->mModelRot, XMMatrixRotationX(rot));
	XMStoreFloat4x4(&this->mRotMatrix, XMLoadFloat4x4(&mModelRot));
	this->CalcWorld();
}

void VirtualEntity::RotateY(float rot)
{
	XMStoreFloat4x4(&this->mModelRot, XMMatrixRotationY(rot));
	XMStoreFloat4x4(&this->mRotMatrix, XMLoadFloat4x4(&mModelRot));
	this->CalcWorld();
}

void VirtualEntity::RotateZ(float rot)
{
	XMStoreFloat4x4(&this->mModelRot, XMMatrixRotationZ(rot));
	XMStoreFloat4x4(&this->mRotMatrix, XMLoadFloat4x4(&mModelRot));
	this->CalcWorld();
}

void VirtualEntity::RotateXYZ(XMFLOAT3 rot)
{
	XMStoreFloat4x4(&this->mModelRot,
		XMMatrixRotationX(rot.x) *
		XMMatrixRotationY(rot.y) *
		XMMatrixRotationZ(rot.z));
	this->CalcWorld();
}

void VirtualEntity::SetPosition(XMFLOAT3 position)
{
	this->mPosition = position;
	XMStoreFloat4x4(&this->mModelOffset, XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z));
	this->CalcWorld();
}

void VirtualEntity::SetScale(float scale)
{
	XMStoreFloat4x4(&this->mModelScale, XMMatrixScaling(scale, scale, scale));
	this->CalcWorld();
}