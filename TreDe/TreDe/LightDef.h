//-------------------------------------------------------------------------------------------
// File: LightDef.h
//
// This class is only used to define lights
//-------------------------------------------------------------------------------------------
#pragma once

#ifndef LIGHTDEF_H
#define LIGHTDEF_H

#include <windows.h>
#include <xnamath.h>

struct DirectionalLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Padding;

	DirectionalLight()
	{ 
		ZeroMemory(this, sizeof(this)); 
	}
};

struct PointLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;

	PointLight()
	{ 
		ZeroMemory(this, sizeof(this)); 
	}
};

struct SpotLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;
	
	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Spot;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;

	SpotLight() 
	{
		ZeroMemory(this, sizeof(this));
	}
};

struct Material
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;

	Material()
	{ 
		ZeroMemory(this, sizeof(this)); 
	}
};

namespace LightLights
{
	const XMFLOAT4 WHITE(1.0f, 1.0f, 1.0f, 1.0f);
	const XMFLOAT4 BLACK(0.0f, 0.0f, 0.0f, 1.0f);
	const XMFLOAT4 RED(1.0f, 0.0f, 0.0f, 1.0f);
	const XMFLOAT4 GREEN(0.0f, 1.0f, 0.0f, 1.0f);
	const XMFLOAT4 BLUE(0.0f, 0.0f, 1.0f, 1.0f);
	const XMFLOAT4 YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
	const XMFLOAT4 ORANGE(1.0f, 0.5f, 0.0f, 1.0f);
	const XMFLOAT4 CYAN(0.0f, 1.0f, 1.0f, 1.0f);
	const XMFLOAT4 MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);

	const XMFLOAT4 BEACH_SAND(1.0f, 0.96f, 0.62f, 1.0f);
	const XMFLOAT4 LIGHT_YELLOW_GREEN(0.48f, 0.77f, 0.46f, 1.0f);
	const XMFLOAT4 DARK_YELLOW_GREEN(0.1f, 0.48f, 0.19f, 1.0f);
	const XMFLOAT4 DARKBROWN(0.45f, 0.39f, 0.34f, 1.0f);
	const XMFLOAT4 WINDOWS_BLUE(0.0f, 0.2f, 0.4f, 1.0f);
}
#endif