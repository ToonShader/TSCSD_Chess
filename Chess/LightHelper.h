#pragma once

#include <Windows.h>
#include <xnamath.h>

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(*this)); } // TODO: Check that this works vs (this)

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float pad;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(*this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Att;
	float pad;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(*this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float SpotFactor;

	XMFLOAT3 Att;
	float pad;
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(*this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;
};