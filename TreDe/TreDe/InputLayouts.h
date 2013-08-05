#ifndef INPUTLAYOUTS_H
#define INPUTLAYOUTS_H
#include "VertexDef.h"

class InputLayouts
{
private:
	class InputLayoutDesc
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC Pos[1];
		static const D3D11_INPUT_ELEMENT_DESC PosNor[2];
		static const D3D11_INPUT_ELEMENT_DESC PosNorTex[3];
		static const D3D11_INPUT_ELEMENT_DESC PosNorTexTan[4];
		static const D3D11_INPUT_ELEMENT_DESC Particle[5];
		static const D3D11_INPUT_ELEMENT_DESC PosNorTexTanSkin[6];
	};

public:
	static void Initialize(ID3D11Device* device);
	static void Shutdown();

public:
	static ID3D11InputLayout* mPos;
	static ID3D11InputLayout* mPosNor;
	static ID3D11InputLayout* mPosNorTex;
	static ID3D11InputLayout* mPosNorTexTan;
	static ID3D11InputLayout* mParticle;
	static ID3D11InputLayout* mPosNorTexTanSkin;
};
#endif