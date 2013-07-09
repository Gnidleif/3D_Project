//-------------------------------------------------------------------------------------------
// File: LightDef.h
//
// This class is only used to define lights
//-------------------------------------------------------------------------------------------

#ifndef LIGHTDEF_H
#define LIGHTDEF_H

#include <windows.h>
#include <xnamath.h>

struct DirectionalLight
{
	DirectionalLight()
		:
		Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Direction(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		Padding(1.0f)
	{ 
		ZeroMemory(this, sizeof(this)); 
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Padding;
};

struct PointLight
{
	PointLight()
		:
		Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),

		Position(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		Range(1000.0f),

		Attenuation(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		Padding(1.0f)
	{ 
		ZeroMemory(this, sizeof(this)); 
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};

struct SpotLight
{
	SpotLight()
		:
		Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),

		Position(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		Range(1000.0f),
		Direction(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		Spot(1.0f),

		Attenuation(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		Padding(1.0f)
	{ 
		ZeroMemory(this, sizeof(this)); 
	}

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
};

struct Material
{
	Material()
		:
		Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Reflect(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
	{ 
		ZeroMemory(this, sizeof(this)); 
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

#endif