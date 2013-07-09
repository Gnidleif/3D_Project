#ifndef VIRTUALENTITY_H
#define VIRTUALENTITY_H
#include "d3dUtilities.h"
#include "Camera.h"
#include "ModelClass.h"

class VirtualEntity
{
public:
	VirtualEntity();
	virtual ~VirtualEntity();
	virtual void Initialize(XMFLOAT3 position, float scale);
	virtual void Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera) = 0;

	void RotateX(float rot);
	void RotateY(float rot);
	void RotateZ(float rot);
	void RotateXYZ(XMFLOAT3 rot);
	void RotateRollPitchYaw(XMFLOAT3 RPY);
	
	void SetPosition(XMFLOAT3 position);
	void SetScale(float scale);

private:
	virtual void CalcWorld() = 0;

protected:
	XMFLOAT3 mPosition;
	float mRotation;

	XMFLOAT4X4 mRotMatrix;
	XMFLOAT4X4 mModelScale;
	XMFLOAT4X4 mModelRot;
	XMFLOAT4X4 mModelOffset;
};
#endif