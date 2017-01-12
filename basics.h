#ifndef BASICS_H
#define BASICS_H

#include <cmath>
#include <iostream>
using namespace std;

#define PIXELWIDTH 800
#define PIXELHEIGHT 600
#define SCREENWIDTH 8.0
#define SCREENHEIGHT 6.0
#define FOCALLENGTH 5

#define EPSILON 0.0001f

class Vector3
{
private:
	double v[3];

public:
	Vector3() { v[0] = v[1] = v[2] = 0; }
	Vector3(const double & value) { v[0] = v[1] = v[2] = value; }
	Vector3(const double & a, const double & b, const double &c)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
	}
	Vector3(double arr[])
	{
		v[0] = arr[0];
		v[1] = arr[1];
		v[2] = arr[2];
	}
	Vector3(const Vector3 & right)
	{
		v[0] = right.v[0];
		v[1] = right.v[1];
		v[2] = right.v[2];
	}

	const double & X() const { return v[0]; }
	const double & Y() const { return v[1]; }
	const double & Z() const { return v[2]; }
	double & X() { return v[0]; }
	double & Y() { return v[1]; }
	double & Z() { return v[2]; }
	void Set(const double & a, const double & b, const double &c)	
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
	}
	const double & operator[] (int index) const { return v[index]; }
	double & operator[] (int index) { return v[index]; }

	Vector3 operator+ (const Vector3 & r) const { return Vector3(v[0] + r[0], v[1] + r[1], v[2] + r[2]); }
	Vector3 operator- (const Vector3 & r) const { return Vector3(v[0] - r[0], v[1] - r[1], v[2] - r[2]); }
	Vector3 operator* (const Vector3 & r) const { return Vector3(v[0] * r[0], v[1] * r[1], v[2] * r[2]); }
	Vector3 operator* (const double & r) const { return Vector3(v[0] * r, v[1] * r, v[2] * r); }
	Vector3 operator/ (const double & r) const { return Vector3(v[0] / r, v[1] / r, v[2] / r); }
	Vector3 operator- () const { return Vector3(-v[0], -v[1], -v[2]); }
	Vector3 & operator= (const Vector3 & r) { v[0] = r[0]; v[1] = r[1]; v[2] = r[2]; return *this; }
	Vector3 & operator+= (const Vector3 & r) { return (*this) = (*this) + r; }
	Vector3 & operator-= (const Vector3 & r) { return (*this) = (*this) - r; }
	Vector3 & operator*= (const double & r) { return (*this) = (*this) * r; }
	Vector3 & operator/= (const double & r) { return (*this) = (*this) / r; }

	double Dot(const Vector3 & r) const
	{ 
		return v[0] * r[0] + v[1] * r[1] + v[2] * r[2]; 
	}
	Vector3 Cross(const Vector3 & r) const
	{
		return Vector3( v[1] * r[2] - v[2] * r[1],
						v[2] * r[0] - v[0] * r[2],
						v[0] * r[1] - v[1] * r[0] );
	}
	double L1Norm() const
	{
		return fabs(v[0]) + fabs(v[1]) + fabs(v[2]);
	}
	double L2Norm() const
	{
		return sqrt(Dot(*this));
	}
	double Distance(const Vector3 & r) const
	{
		return (*this - r).L2Norm();
	}

	void Normalize()
	{
		double denom = this->L2Norm();
		v[0] /= denom;
		v[1] /= denom;
		v[2] /= denom;
	}
	Vector3 Normalized()
	{
		Vector3 tmp_v;
		double denom = this->L2Norm();
		tmp_v[0] /= denom;
		tmp_v[1] /= denom;
		tmp_v[2] /= denom;
		return tmp_v;
	}
	Vector3 Min(const Vector3 & r)
	{
		return Vector3(__min(v[0], r[0]), __min(v[1], r[1]), __min(v[2], r[2]));
	}
	Vector3 Max(const Vector3 & r)
	{
		return Vector3(__max(v[0], r[0]), __max(v[1], r[1]), __max(v[2], r[2]));
	}
	const double * ToArray() const
	{
		return v;
	}
};

typedef Vector3 Direction;
typedef Vector3 Point;
typedef Vector3 Color;
typedef unsigned int Pixel;

#endif // BASICS_H