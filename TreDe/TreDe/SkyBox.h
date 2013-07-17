#ifndef SKYBOX_H
#define SKYBOX_H
#include "d3dUtilities.h"
#include "Camera.h"
#include "TextureClass.h"
#include "VertexDef.h"
#include "GeometryCreator.h"
#include "InputLayouts.h"
#include "Effects.h"

class SkyBox
{
public:
	SkyBox();
	~SkyBox();
	void Initialize(ID3D11Device* device, float radius);
	void Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera);

public:
	ID3D11ShaderResourceView* GetCubeMap() const { return this->mCubeMapSRV; }

private:
	void CreateSphere(float radius);

private:
	std::string mFileName;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	ID3D11ShaderResourceView* mCubeMapSRV;
	std::vector<UINT> mIndices;
	UINT mIndexCount;
};
#endif