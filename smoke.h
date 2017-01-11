#pragma once
#include <vector>
#include "basics.h"
#include "Voxel.h"

using namespace VolRender;

#define VOXEL_SCALE 0.01f  //must be evenly divisible into 1

class Sphere;

class Smoke
{
public:
	Smoke();
	~Smoke();

	void GenerateVoxels(float maxX, float maxY, float maxZ);
	void VoxelizeSpheres(std::vector<Sphere> spheres);
	//bool IsOutside(Vector3& location);
	//void AddLightTransmissivity(Light light);
	//float InterpAllValues(Vector3& location, Vector3* color, float* trans);
private:
	std::vector<Voxel> m_voxelarray;
	float m_maxX;
	float m_maxY;
	float m_maxZ;
	unsigned int m_voxelWidth;
	unsigned int m_voxelHeight;
	unsigned int m_voxelDepth;
};