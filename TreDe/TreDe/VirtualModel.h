#ifndef VIRTUALMODEL_H
#define VIRTUALMODEL_H
#include "d3dUtilities.h"
#include "TextureClass.h"
#include "xnacollision.h"

class VirtualModel
{
public:
	VirtualModel();
	virtual ~VirtualModel();

private:
	virtual void CreateMatsAndMeshes(std::string filename) = 0;

protected:
	UINT mMeshCount;
	UINT mMaterialCount;
};
#endif