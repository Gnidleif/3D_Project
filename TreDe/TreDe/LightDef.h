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
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Padding;

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

	DirectionalLight(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT3 direction, float padding)
		: Ambient(ambient), Diffuse(diffuse), Specular(specular), Direction(direction), Padding(padding)
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

	PointLight(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, 
		XMFLOAT3 position, float range, XMFLOAT3 attenuation, float padding)
		: Ambient(ambient), Diffuse(diffuse), Specular(specular), Position(position), Range(range), Attenuation(attenuation), Padding(padding)
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

	SpotLight(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, 
		XMFLOAT3 position, float range, XMFLOAT3 direction, float spot, XMFLOAT3 attenuation, float padding)
		: Ambient(ambient), Diffuse(diffuse), Specular(specular),
		Position(position), Range(range), Direction(direction), Spot(spot), Attenuation(attenuation), Padding(padding) 
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
		:
		Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		Reflect(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
	{ 
		ZeroMemory(this, sizeof(this)); 
	}

	Material(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT4 reflect)
		:
		Ambient(ambient), Diffuse(diffuse), Specular(specular), Reflect(reflect)
	{
		ZeroMemory(this, sizeof(this)); 
	}
};

#endif