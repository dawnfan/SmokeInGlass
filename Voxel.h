//Voxel.h
//Travis Gorkin
//Copyright (c)2009

#ifndef INCLUDED_VOXEL_H
#define INCLUDED_VOXEL_H

#include "basics.h"

namespace VolRender
{
class Voxel
{
public:
	Voxel(float x, float y, float z);

	Vector3 GetCenter() { return m_center;}
	float GetDensity() { return m_density;}
	void AddDensity(float density);
	Vector3 GetColor() { return m_color;}
	void AddColor(Vector3 color);
	double GetNoise() {return m_noise;}
	void SetNoise(double noise) {m_noise = noise;}
	float GetTransmissivity() {return m_transmissivity;}
	void SetTransmissivity(float trans) {m_transmissivity = trans;}
	
private:
	Vector3 m_center;
	float m_density;
	Vector3 m_color;
	double m_noise;
	float m_transmissivity;

};
}

#endif