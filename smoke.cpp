#pragma once
#include "smoke.h"
#include "Voxel.h"
#include "perlin.h"
#include "image.h"

using namespace VolRender;

Smoke::Smoke()
{
}

Smoke::~Smoke()
{
}

void Smoke::GenerateVoxels(float maxX, float maxY, float maxZ)
{
	m_maxX = ceil(maxX);
	m_maxY = ceil(maxY);
	m_maxZ = ceil(maxZ);

	m_voxelWidth = (unsigned int)(ceil((2 * m_maxX) / VOXEL_SCALE)) + 1;
	m_voxelHeight = (unsigned int)(ceil((2 * m_maxY) / VOXEL_SCALE)) + 1;
	m_voxelDepth = (unsigned int)(ceil(m_maxZ / VOXEL_SCALE)) + 1;

	float x, y, z;

	Voxel voxel(0.0f, 0.0f, 0.0f);

	double alpha = 1.0;
	double beta = 2.0;
	for (unsigned int i = 0; i < m_voxelDepth; i++)
	{
		z = -1.0f*i*VOXEL_SCALE;
		for (unsigned int j = 0; j < m_voxelHeight; j++)
		{
			y = m_maxY - (j*VOXEL_SCALE);
			for (unsigned int k = 0; k < m_voxelWidth; k++)
			{
				x = m_maxX - (k*VOXEL_SCALE);
				voxel = Voxel(x, y, z);
				double d = PerlinNoise3D(x, y, z, alpha, beta, 6);
				voxel.SetNoise(d);
				m_voxelarray.push_back(voxel);
			}
		}
	}
}

//Discretizes the color and density of each sphere's volume into the voxels in the voxelspace
void Smoke::VoxelizeSpheres(std::vector<Sphere> spheres)
{
	Vector3 sphereCenter, voxelCenter;
	float sphereRad;
	float radSquared;
	float distance;
	float density;
	Vector3 color;

	for (unsigned int sphereIndex = 0; sphereIndex < spheres.size(); sphereIndex++)
	{
		sphereCenter = spheres[sphereIndex].getCenter();
		density = spheres[sphereIndex].getDensity();
		color = spheres[sphereIndex].getMaterial()->getColor();
		for (unsigned int voxelIndex = 0; voxelIndex < m_voxelarray.size(); voxelIndex++)
		{
			voxelCenter = m_voxelarray[voxelIndex].GetCenter();
			sphereRad = spheres[sphereIndex].getRadius();
			sphereRad = (float)m_voxelarray[voxelIndex].GetNoise()*0.5f *sphereRad + sphereRad;
			radSquared = sphereRad*sphereRad;
			distance = 1.0f - (pow((voxelCenter - sphereCenter).L2Norm(), 2) / radSquared);
			if (distance > 0.0f)
			{
				m_voxelarray[voxelIndex].AddColor(color);
				if ((density > 0.0f) && (density <= 1.0f))
					m_voxelarray[voxelIndex].AddDensity(density);
				else if (density > 1.0f)
					m_voxelarray[voxelIndex].AddDensity(distance*density);
			}
		}
	}

}
