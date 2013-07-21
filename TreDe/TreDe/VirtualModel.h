#ifndef VIRTUALMODEL_H
#define VIRTUALMODEL_H
#include "d3dUtilities.h"
#include "TextureClass.h"
#include "xnacollision.h"

class VirtualModel
{
public:
	VirtualModel(ID3D11Device* device);
	virtual ~VirtualModel();

public:
	UINT GetMeshCount() const { return this->mMeshCount; }
	UINT GetMaterialCount() const { return this->mMaterialCount; }

private:
	virtual void CreateMatsAndMeshes(std::string filename) = 0;
	virtual void CreateBuffers() = 0;

protected:
	ID3D11Device* mDevice;
	UINT mMeshCount;
	UINT mMaterialCount;
};
#endif