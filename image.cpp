#include "image.h"

int Sphere::intersect(Ray& ray, double& t)
{
	int result = MISS;
	Vector3 o = ray.getOrigin();
	Vector3 d = ray.getDirection();
	Vector3 v = o - my_center;
	double b, ac, delta;
	b = v.Dot(d);
	ac = v.Dot(v) - my_Radius*my_Radius;
	delta = b*b - ac;
	if (delta > 0)
	{
		delta = sqrt(delta);
		double t1 = -b - delta;
		double t2 = -b + delta;
		if (t2 > 0)
		{
			if (t1 < 0)
			{
				if (t2 < t)
				{
					t = t2;
					result = INPRIM;
				}
			}
			else
			{
				if (t1 < t)
				{
					t = t1;
					result = HIT;
				}
			}
		}
	}
	return result;
}

int Plane::intersect(Ray& ray, double& t)
{
	int result = MISS;
	double d = ray.getDirection().Dot(my_normal);
	if (abs(d) > EPSILON)
	{
		double t_tmp = -(ray.getOrigin().Dot(my_normal) + my_d) / d;
		if (t_tmp > 0)
		{
			if (t_tmp < t)
			{
				t = t_tmp;
				result = HIT;
			}
		}
	}
	return result;
}

void Image::init()
{
	//my_primitive = new Primitive*[100];
	//
	//my_primitive[0] = new Plane(Vector3(0.0f, 1.0f, 0.0f), 4.4f);
	//my_primitive[0]->getMaterial()->setReflection(0);
	//my_primitive[0]->getMaterial()->setDiffusion(1.0f);
	//my_primitive[0]->getMaterial()->setRefraction(0.0f);
	//my_primitive[0]->getMaterial()->setColor(Color(0.9f, 0.2f, 0.1f));

	//my_primitive[1] = new Plane(Vector3(1.0f, 0.0f, 0.0f), 10.0f);
	//my_primitive[1]->getMaterial()->setReflection(0.5);
	//my_primitive[1]->getMaterial()->setRefraction(0.8f);
	//my_primitive[1]->getMaterial()->setRefrIndex(1.3f);
	//my_primitive[1]->getMaterial()->setColor(Color(0.9f, 0.9f, 0.1f));

	//my_primitive[2] = new Sphere(Vector3(3.5f, -0.5f, -5.0f), 3.0f);
	//my_primitive[2]->getMaterial()->setReflection(1.0f);
	//my_primitive[2]->getMaterial()->setRefraction(0.0f);
	//my_primitive[2]->getMaterial()->setRefrIndex(1.3f);
	//my_primitive[2]->getMaterial()->setDiffusion(0.1f);
	//my_primitive[2]->getMaterial()->setColor(Color(0.2f, 0.2f, 1.0f));

	//my_primitive[3] = new Sphere(Vector3(0.0f, 5.0f, 5.0f), 0.1f);
	//my_primitive[3]->setLight(true);
	//my_primitive[3]->getMaterial()->setColor(Color(0.9f, 0.9f, 0.9f));

	//number = 4;

	my_primitive = new Primitive*[500];
	// ground plane
	my_primitive[0] = new Plane(Vector3(0, 1, 0), 4.4f);
	my_primitive[0]->getMaterial()->setReflection(0.0f);
	my_primitive[0]->getMaterial()->setRefraction(0.0f);
	my_primitive[0]->getMaterial()->setDiffusion(1.0f);
	my_primitive[0]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	// big sphere
	my_primitive[1] = new Sphere(Vector3(2, 0.8f, 3), 2.5f);
	my_primitive[1]->getMaterial()->setReflection(0.2f);
	my_primitive[1]->getMaterial()->setRefraction(0.8f);
	my_primitive[1]->getMaterial()->setRefrIndex(1.3f);
	my_primitive[1]->getMaterial()->setColor(Color(0.7f, 0.7f, 0.7f));
	// inside big sphere
	my_primitive[8] = new Sphere(Vector3(2, 0.8f, 3), 2.2f);
	my_primitive[8]->getMaterial()->setReflection(0.2f);
	my_primitive[8]->getMaterial()->setRefraction(0.8f);
	my_primitive[8]->getMaterial()->setRefrIndex(1.0f);
	my_primitive[8]->getMaterial()->setColor(Color(0.7f, 0.7f, 0.7f));
	// small sphere
	my_primitive[2] = new Sphere(Vector3(-5.5f, -0.5, 7), 2);
	my_primitive[2]->getMaterial()->setReflection(0.5f);
	my_primitive[2]->getMaterial()->setRefraction(0.0f);
	my_primitive[2]->getMaterial()->setRefrIndex(1.3f);
	my_primitive[2]->getMaterial()->setDiffusion(0.1f);
	my_primitive[2]->getMaterial()->setColor(Color(0.7f, 0.7f, 1.0f));
	// light source 1
	my_primitive[3] = new Sphere(Vector3(0, 5, 5), 0.1f);
	my_primitive[3]->setLight(true);
	my_primitive[3]->getMaterial()->setColor(Color(0.4f, 0.4f, 0.4f));
	// light source 2
	my_primitive[4] = new Sphere(Vector3(-3, 5, 1), 0.1f);
	my_primitive[4]->setLight(true);
	my_primitive[4]->getMaterial()->setColor(Color(0.6f, 0.6f, 0.8f));
	// extra sphere
	my_primitive[5] = new Sphere(Vector3(-1.5f, -3.8f, 1), 1.5f);
	my_primitive[5]->getMaterial()->setReflection(0.0f);
	my_primitive[5]->getMaterial()->setRefraction(0.8f);
	my_primitive[5]->getMaterial()->setRefrIndex(1.3f);
	my_primitive[5]->getMaterial()->setColor(Color(1.0f, 0.4f, 0.4f));
	// back plane
	my_primitive[6] = new Plane(Vector3(0.4f, 0, -1), 12);
	my_primitive[6]->getMaterial()->setReflection(0.0f);
	my_primitive[6]->getMaterial()->setRefraction(0.0f);
	my_primitive[6]->getMaterial()->setSpecular(0);
	my_primitive[6]->getMaterial()->setDiffusion(0.6f);
	my_primitive[6]->getMaterial()->setColor(Color(0.5f, 0.3f, 0.5f));
	// ceiling plane
	my_primitive[7] = new Plane(Vector3(0, -1, 0), 7.4f);
	my_primitive[7]->getMaterial()->setReflection(0.0f);
	my_primitive[7]->getMaterial()->setRefraction(0.0f);
	my_primitive[7]->getMaterial()->setSpecular(0);
	my_primitive[7]->getMaterial()->setDiffusion(0.5f);
	my_primitive[7]->getMaterial()->setColor(Color(0.4f, 0.7f, 0.7f));
	// grid
	int prim = 9;
	for (int x = 0; x < 8; x++) for (int y = 0; y < 7; y++)
	{
		my_primitive[prim] = new Sphere(Vector3(-4.5f + x * 1.5f, -4.3f + y * 1.5f, 10), 0.3f);
		my_primitive[prim]->getMaterial()->setReflection(0);
		my_primitive[prim]->getMaterial()->setRefraction(0);
		my_primitive[prim]->getMaterial()->setSpecular(0.6f);
		my_primitive[prim]->getMaterial()->setDiffusion(0.6f);
		my_primitive[prim]->getMaterial()->setColor(Color(0.3f, 1.0f, 0.4f));
		prim++;
	}
	// set number of primitives
	number = prim;
}