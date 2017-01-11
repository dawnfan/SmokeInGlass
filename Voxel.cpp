//Voxel.cpp
//Travis Gorkin
//Copyright (c)2009

#include "Voxel.h"

using namespace VolRender;

Voxel::Voxel(float x, float y, float z)
	:m_center(Vector3(x, y, z)), m_density(0.0f), m_transmissivity(0.0f)
{
}

void Voxel::AddDensity(float density)
{
	 m_density += density;
}

void Voxel::AddColor(Vector3 color)
{
	m_color += color;
}