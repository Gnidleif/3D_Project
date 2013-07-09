#ifndef GEOMETRYCREATOR_H
#define GEOMETRYCREATOR_H
#include "VertexDef.h"

class GeometryCreator
{
public:
	static void CreateSphere(float radius, UINT slice, UINT stack, std::vector<VertexDef::PosNorTexTan>& vertices, std::vector<UINT>& indices);
};
#endif

