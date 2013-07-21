#include "VirtualModel.h"
using namespace std;

VirtualModel::VirtualModel(ID3D11Device* device)
	:
	mMeshCount(0), mMaterialCount(0), mDevice(device)
{
}

VirtualModel::~VirtualModel(void)
{
}
