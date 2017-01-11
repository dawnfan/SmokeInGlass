#ifndef RAY_H
#define RAY_H

#include "basics.h"

class Ray
{
private:
	Vector3 my_origin;
	Vector3 my_direction;
public:
	Ray() : my_origin(Vector3(0, 0, 0)), my_direction(Vector3(0, 0, 0)){};
	Ray(Vector3& origin, Vector3& direction) : my_origin(origin), my_direction(direction){ my_direction.Normalize(); }
	void setOrigin(Vector3& origin) { my_origin = origin; }
	void setDirection(Vector3& direction) { my_direction = direction; }
	Vector3& getOrigin() { return my_origin; }
	Vector3& getDirection() { return my_direction; }
	Vector3& getPoint(double t) { return my_origin + my_direction * t; }
};

#endif	//	RAY_H