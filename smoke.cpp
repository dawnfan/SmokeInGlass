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

void Smoke::GenerateVoxels(Vector3 maxV, Vector3 minV)
{
	m_maxX = ceil(maxV.X());
	m_maxY = ceil(maxV.Y());
	m_maxZ = ceil(maxV.Z());
	m_minX = ceil(minV.X());
	m_minY = ceil(minV.Y());
	m_minZ = ceil(minV.Z());
	// set the voxel distribution(cube) based on smoke size
	Vector3 voxelSize = maxV - minV;
	m_voxelWidth = (unsigned int)(ceil(voxelSize.X() / VOXEL_SCALE)) + 1;
	m_voxelHeight = (unsigned int)(ceil(voxelSize.Y() / VOXEL_SCALE)) + 1;
	m_voxelDepth = (unsigned int)(ceil(voxelSize.Z() / VOXEL_SCALE)) + 1;

	float x, y, z;

	Voxel voxel(0.0f, 0.0f, 0.0f);
	// generate the voxel-array spread in space.
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
void Smoke::VoxelizeSpheres(std::vector<Sphere*> spheres)
{
	Vector3 sphereCenter, voxelCenter;
	float sphereRad;
	float radSquared;
	float distance;
	float density;
	Vector3 color;

	for (unsigned int sphereIndex = 0; sphereIndex < spheres.size(); sphereIndex++)
	{
		sphereCenter = spheres[sphereIndex]->getCenter();
		density = spheres[sphereIndex]->getDensity();
		color = spheres[sphereIndex]->getMaterial()->getColor();
		for (unsigned int voxelIndex = 0; voxelIndex < m_voxelarray.size(); voxelIndex++)
		{
			// calculate distance between voxel and sphere center.
			voxelCenter = m_voxelarray[voxelIndex].GetCenter();
			sphereRad = spheres[sphereIndex]->getRadius();
			sphereRad = (float)m_voxelarray[voxelIndex].GetNoise()*0.5f *sphereRad + sphereRad;
			radSquared = sphereRad*sphereRad;
			Vector3 n_distance = voxelCenter - sphereCenter;
			n_distance.Normalize();
			distance = 1.0f - (pow(n_distance.L2Norm(), 2) / radSquared);
			// set color based on distance
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
//Calculates light transmissivity for every voxel in the voxelspace
void Smoke::AddLightTransmissivity(std::vector<Sphere*> lights)
{
	Vector3 voxelCenter;
	Vector3 lightDir;
	float T = 0.0f;
	int light_num = lights.size();
	for (unsigned int i = 0; i < m_voxelarray.size(); i++)
	{
		T = 0.0f;
		for (int l_t = 0; l_t < light_num; l_t++)
		{
			voxelCenter = m_voxelarray[i].GetCenter();
			lightDir = lights[l_t]->getCenter() - voxelCenter;
			T += RaymarchLight(voxelCenter, lightDir);
		}
		T /= light_num;
		m_voxelarray[i].SetTransmissivity(T);
	}

}

//Calculates the density, color, and light transmissivity at a location in the voxel space using 
// trilinear interpolation from eight closest neighbor voxels
float Smoke::InterpAllValues(Vector3& location, Vector3* pColor, float* pTrans)
{
	float x = location.X();
	float y = location.Y();
	float z = location.Z();

	bool outside = IsOutside(location);
	unsigned int index = GetIndexFromLocation(x, y, z);
	Vector3 center = m_voxelarray[index].GetCenter();

	if (index + m_voxelHeight*m_voxelWidth > m_voxelarray.size())
		return 0.0f;

	Voxel voxel0 = m_voxelarray[index];
	Voxel voxel1 = m_voxelarray[index - 1];
	Voxel voxel2 = m_voxelarray[index - m_voxelWidth];
	Voxel voxel3 = m_voxelarray[index - m_voxelWidth - 1];
	Voxel voxel4 = m_voxelarray[index + m_voxelHeight*m_voxelWidth];
	Voxel voxel5 = m_voxelarray[index + m_voxelHeight*m_voxelWidth - 1];
	Voxel voxel6 = m_voxelarray[index + m_voxelHeight*m_voxelWidth - m_voxelWidth];
	Voxel voxel7 = m_voxelarray[index + m_voxelHeight*m_voxelWidth - m_voxelWidth - 1];

	float weight0 = (location - voxel0.GetCenter()).L2Norm();
	float weight1 = (location - voxel1.GetCenter()).L2Norm();
	float weight2 = (location - voxel2.GetCenter()).L2Norm();
	float weight3 = (location - voxel3.GetCenter()).L2Norm();
	float weight4 = (location - voxel4.GetCenter()).L2Norm();
	float weight5 = (location - voxel5.GetCenter()).L2Norm();
	float weight6 = (location - voxel6.GetCenter()).L2Norm();
	float weight7 = (location - voxel7.GetCenter()).L2Norm();

	float weightSum = weight0 + weight1 + weight2 + weight3 + weight4 + weight5 + weight6 + weight7;
	weight0 = weight0 / weightSum;
	weight1 = weight1 / weightSum;
	weight2 = weight2 / weightSum;
	weight3 = weight3 / weightSum;
	weight4 = weight4 / weightSum;
	weight5 = weight5 / weightSum;
	weight6 = weight6 / weightSum;
	weight7 = weight7 / weightSum;

	Vector3 color = voxel0.GetColor()*weight0 + voxel1.GetColor()*weight1 + voxel2.GetColor()*weight2 +
		voxel3.GetColor()*weight3 + voxel4.GetColor()*weight4 + voxel5.GetColor()*weight5 +
		voxel6.GetColor()*weight6 + voxel7.GetColor()*weight7;

	pColor->Set(color.X(), color.Y(), color.Z());

	float trans = weight0*voxel0.GetTransmissivity() + weight1*voxel1.GetTransmissivity() + weight2*voxel2.GetTransmissivity() +
		weight3*voxel3.GetTransmissivity() + weight4*voxel4.GetTransmissivity() + weight5*voxel5.GetTransmissivity() +
		weight6*voxel6.GetTransmissivity() + weight7*voxel7.GetTransmissivity();

	*(pTrans) = trans;

	return weight0*voxel0.GetDensity() + weight1*voxel1.GetDensity() + weight2*voxel2.GetDensity() +
		weight3*voxel3.GetDensity() + weight4*voxel4.GetDensity() + weight5*voxel5.GetDensity() +
		weight6*voxel6.GetDensity() + weight7*voxel7.GetDensity();

}



//Returns true if the location is outside of the voxel space
bool Smoke::IsOutside(Vector3& location)
{
	float x = fabs(location.X());
	float y = fabs(location.Y());
	float z = location.Z();
	if ((x >= m_maxX) || (x <= m_minX) || (y >= m_maxY) || (y <= m_minY) || (z >= m_maxZ) || (z <= m_minZ))
		return true;
	else
		return false;
}

//Calculates the density at a location in the voxel space using trilinear interpolation from eight closest neighbor voxels
float Smoke::InterpDensity(Vector3& location)
{
	float x = location.X();
	float y = location.Y();
	float z = location.Z();

	bool outside = IsOutside(location);
	unsigned int index = GetIndexFromLocation(x, y, z);
	Vector3 center = m_voxelarray[index].GetCenter();

	Voxel voxel0 = m_voxelarray[index];
	Voxel voxel1 = m_voxelarray[index - 1];
	Voxel voxel2 = m_voxelarray[index - m_voxelWidth];
	Voxel voxel3 = m_voxelarray[index - m_voxelWidth - 1];
	Voxel voxel4 = m_voxelarray[index + m_voxelHeight*m_voxelWidth];
	Voxel voxel5 = m_voxelarray[index + m_voxelHeight*m_voxelWidth - 1];
	Voxel voxel6 = m_voxelarray[index + m_voxelHeight*m_voxelWidth - m_voxelWidth];
	Voxel voxel7 = m_voxelarray[index + m_voxelHeight*m_voxelWidth - m_voxelWidth - 1];

	float weight0 = (location - voxel0.GetCenter()).L2Norm();
	float weight1 = (location - voxel1.GetCenter()).L2Norm();
	float weight2 = (location - voxel2.GetCenter()).L2Norm();
	float weight3 = (location - voxel3.GetCenter()).L2Norm();
	float weight4 = (location - voxel4.GetCenter()).L2Norm();
	float weight5 = (location - voxel5.GetCenter()).L2Norm();
	float weight6 = (location - voxel6.GetCenter()).L2Norm();
	float weight7 = (location - voxel7.GetCenter()).L2Norm();

	float weightSum = weight0 + weight1 + weight2 + weight3 + weight4 + weight5 + weight6 + weight7;
	weight0 = weight0 / weightSum;
	weight1 = weight1 / weightSum;
	weight2 = weight2 / weightSum;
	weight3 = weight3 / weightSum;
	weight4 = weight4 / weightSum;
	weight5 = weight5 / weightSum;
	weight6 = weight6 / weightSum;
	weight7 = weight7 / weightSum;

	return weight0*voxel0.GetDensity() + weight1*voxel1.GetDensity() + weight2*voxel2.GetDensity() +
		weight3*voxel3.GetDensity() + weight4*voxel4.GetDensity() + weight5*voxel5.GetDensity() +
		weight6*voxel6.GetDensity() + weight7*voxel7.GetDensity();

}

//Helper function to convert a location from x, y, z coordinates in the voxelspace into an index into the voxelarray vector
unsigned int Smoke::GetIndexFromLocation(float x, float y, float z)
{
	/*if(fabsf(x) > m_maxX)
	return 0;
	if(fabsf(y) > m_maxY)
	return 0;
	if(-z > m_maxZ)
	return 0;*/

	float xV = (m_maxX - x) / VOXEL_SCALE;
	float yV = (m_maxY - y) / VOXEL_SCALE;
	float zV = (m_maxZ - z) / VOXEL_SCALE;

	unsigned int xInt = m_voxelWidth - 1 - (unsigned int)xV;
	unsigned int yInt = (m_voxelHeight - 1) - (unsigned int)yV;
	unsigned int zInt = m_voxelDepth - 1 - (unsigned int)zV;

	return xInt + yInt*m_voxelWidth + zInt*m_voxelHeight*m_voxelWidth;
}

//Simplified raymarch algorithm used to compute the light transmissivity, or percent of incoming light that reaches a voxel
float Smoke::RaymarchLight(Vector3& voxelCenter, Vector3& lightDir)
{
	float T = 1.0f;
	float deltaT;
	Vector3 location = voxelCenter;
	float deltaS = 0.1f;
	float instDensity;

	while (T > 0)
	{
		location = location + lightDir*deltaS;
		if (IsOutside(location))
			return T;
		else
		{
			instDensity = InterpDensity(location);
			deltaT = exp(-deltaS*instDensity);
			T *= deltaT;
		}
	}

	return T;
}

