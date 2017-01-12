#include "camera.h"

#define MAX_FLOAT_VALUE (static_cast<float>(10e10))

Camera::Camera()
{
	my_screen_width = SCREENWIDTH;
	my_screen_height = SCREENHEIGHT;
	my_pixel_width = PIXELWIDTH;
	my_pixel_height = PIXELHEIGHT;
	start_x = -my_screen_width / 2.0f;
	start_y = -my_screen_height / 2.0f;
	dx = my_screen_width / my_pixel_width;
	dy = my_screen_height / my_pixel_height;
	my_flength = 5;
	my_position = Vector3(0, 0, -10);
	my_front = Vector3(0, 0, 1);
	my_up = Vector3(0, 1, 0);
	my_right = my_front.Cross(my_up);
	my_front.Normalize();
	my_up.Normalize();
	my_right.Normalize();
	my_screen_center = my_position + my_front * my_flength;
	my_image = new Image();
	// smoke rendering
	GenerateSpheres();
	GenerateSmoke();
}

void Camera::GenerateSpheres()
{
	for (int i = 0; i < my_image->getNumber(); i++)
	{
		Primitive* object = my_image->getPrimitive(i);
		if (object->getType() == Primitive::SPHERE)
		{
			if (object->isLight())
			{
				my_lights.push_back((Sphere*)object);
			}
			else if (object->isSmoke())
			{
				my_smoke_spheres.push_back((Sphere*)object);
			}
		}
	}
}
//Generate voxel space large enough to hold all spheres, discretize the spheres into the voxel space, then calculate
// the light transmissivity for each voxel
void Camera::GenerateSmoke()
{
	//find maximum x, y, z coordinates of spheres in view space
	float maxX = 0.0f;
	float maxY = 0.0f;
	float maxZ = 0.0f;
	float minX = MAX_FLOAT_VALUE;
	float minY = MAX_FLOAT_VALUE;
	float minZ = MAX_FLOAT_VALUE;
	float x, y, z;
	float maxPerturbation = 1.0f;

	for (unsigned int i = 0; i < my_smoke_spheres.size(); i++)
	{
		Sphere* sphere = my_smoke_spheres[i];
		// get smoke bounding box
		x = sphere->getCenter().X() + maxPerturbation*sphere->getRadius();
		y = sphere->getCenter().Y() + maxPerturbation*sphere->getRadius();
		z = sphere->getCenter().Z() + maxPerturbation*sphere->getRadius();

		if (x > maxX)
			maxX = x;
		if (y > maxY)
			maxY = y;
		if (z > maxZ)
			maxZ = z;
		x = sphere->getCenter().X() - maxPerturbation*sphere->getRadius();
		y = sphere->getCenter().Y() - maxPerturbation*sphere->getRadius();
		z = sphere->getCenter().Z() - maxPerturbation*sphere->getRadius();

		if (x < minX)
			maxX = x;
		if (y < minY)
			maxY = y;
		if (z < minZ)
			maxZ = z;
	}

	Vector3 maxV(maxX, maxY, maxZ);
	Vector3 minV(minX, minY, minZ);
	my_smoke_render.GenerateVoxels(maxV, minV);
	my_smoke_render.VoxelizeSpheres(my_smoke_spheres);
	my_smoke_render.AddLightTransmissivity(my_lights);

}


int Camera::rayHitTest(Ray ray, double &t, Primitive* &obj_out, bool isShadow){
	int result = 0;
	for (int i = 0; i < my_image->getNumber(); i++)
	{
		Primitive* tmp_object = my_image->getPrimitive(i);
		int tmp_result = tmp_object->intersect(ray, t);
		if (tmp_result)
		{
			result = tmp_result;
			obj_out = tmp_object;
			// shadow ray then early determination
			if (isShadow && (!tmp_object->isLight())){
				break;
			}
		}
	}
	return result;
}

Color Camera::getBaseColor(Ray ray, double t, Primitive* object)
{
	Color pixel_color;
	// #2 : Render base color
	// set hitting vectors
	Vector3 V = ray.getDirection();
	Vector3 intersection = ray.getPoint(t);
	Vector3 N = object->getNormal(intersection);
	Color object_color = object->getMaterial()->getColor();
	for (int j = 0; j < my_image->getNumber(); j++)
	{
		// #2.1 : Find lights
		Primitive* next_object = my_image->getPrimitive(j);
		if (next_object->isLight())
		{
			Primitive* light = next_object;
			if (light->getType() == Primitive::SPHERE)
			{
				// #2.2 : Shadow Ray
				Vector3 L = ((Sphere*)light)->getCenter() - intersection;
				Vector3 R = L - N * 2.0f * L.Dot(N);
				R.Normalize();
				Color light_color = light->getMaterial()->getColor();
				// shadow
				double shadow = 1.0f;
				double tmp_t = L.L2Norm();
				L.Normalize();
				Ray shadow_ray = Ray(intersection + L*EPSILON, L);
				Primitive* shadow_object;
				rayHitTest(shadow_ray, tmp_t, shadow_object, true);
				shadow = shadow_object->isLight();
				// #2.3 : Calculate pixel color
				// diffusion
				double diffusion = object->getMaterial()->getDiffusion();
				if (diffusion > 0)
				{
					double dot = N.Dot(L);
					if (dot > 0)
					{
						double kd = diffusion * dot * shadow;
						pixel_color += light_color * object_color * kd;
					}
				}
				// specular
				double specular = object->getMaterial()->getSpecular();
				if (specular > 0)
				{
					double dot = V.Dot(R);
					if (dot > 0)
					{
						double ks = pow(dot, 20) * specular * shadow;
						pixel_color += light_color * ks;//why no material color?
					}
				}
			}
		}
	}
	return pixel_color;
}

Primitive* Camera::rayTrace(Ray& ray, Color& pixel_color, int re_depth, double index, double& t)
{
	if (re_depth > TRACINGDEPTH) return 0;

	t = 10000.0f;
	Primitive* object = 0;
	// #1 : Ray Hitting test
	int result = rayHitTest(ray, t, object);

	// hit nothing
	if (!object) return 0;
	// hit a light
	if (object->isLight())
	{
		pixel_color = object->getMaterial()->getColor();
	}
	// hit smoke sphere
	else if (object->isSmoke())
	{
		Vector3 intersectionA = ray.getPoint(t);
		Vector3 direction = ray.getDirection();
		Ray ray_in_smoke(intersectionA + direction*EPSILON, direction);
		double t_out_smoke = 10000.0f;
		int result_out_smoke = object->intersect(ray_in_smoke, t_out_smoke);
		Vector3 intersectionB = ray_in_smoke.getPoint(t_out_smoke);
		double startT = t;
		double endT = (intersectionB - intersectionA).X() / direction.X() + startT;
		if (result_out_smoke == -1)
		{
			pixel_color += rayMarch(direction, startT, endT);/////
		}
		else
		{
			intersectionB = intersectionA;
		}
		///////////////how to put density effect into this
		//cout << t << endl;
		double smoke_t;
		Color smoke_color(0, 0, 0);
		Ray ray_out_smoke(intersectionB + direction*EPSILON, direction);
		rayTrace(ray_out_smoke, smoke_color, re_depth + 1, index, smoke_t);
		double density = object->getDensity();
		pixel_color += smoke_color*density;//////
	}
	// hit a object
	else
	{
		// #2 : Render base color
		pixel_color = getBaseColor(ray, t, object);

		// set hitting vectors
		Vector3 V = ray.getDirection();
		Vector3 intersection = ray.getPoint(t);
		Vector3 N = object->getNormal(intersection);
		// #3 : reflection
		double reflection = object->getMaterial()->getReflection();
		Color object_color = object->getMaterial()->getColor();
		if ((reflection > 0) && (re_depth < TRACINGDEPTH))
		{
			Vector3 re_R = V - N * V.Dot(N) * 2.0f;
			Color re_color(0, 0, 0);
			double re_t;
			rayTrace(Ray(intersection + re_R * EPSILON, re_R), re_color, re_depth + 1, index, re_t);
			pixel_color += object_color * re_color * reflection;//color blend??
		}
		// #4 : refraction
		double refraction = object->getMaterial()->getRefraction();
		if ((refraction>0) && (re_depth < TRACINGDEPTH))
		{
			double rindex = object->getMaterial()->getRefrIndex();
			double n = index / rindex;
			Vector3 N = object->getNormal(intersection) * (double)result;
			double cosI = -N.Dot(V);
			double cosT2 = 1.0f - n*n*(1.0f - cosI*cosI);
			if (cosT2 > 0.0f)
			{
				Vector3 T = V*n + N*(n*cosI - sqrtf(cosT2));
				Color rcol(0, 0, 0);
				double dist;
				rayTrace(Ray(intersection + T*EPSILON, T), rcol, re_depth + 1, rindex, dist);
				Color absorbance = object->getMaterial()->getColor() * 0.15f * (-dist);
				Color transparency = Color(expf(absorbance.X()), expf(absorbance.Y()), expf(absorbance.Z()));
				pixel_color += rcol*transparency;
			}
		}
	}

	return object;
}

Color Camera::rayMarch(Vector3& rayDirection, double startT, double endT)
{
	Color color_a(0, 0, 0);
	Vector3 location(0, 0, 0);
	location = location + rayDirection * startT;
	double T = 1.0f;
	double deltaT;
	double current_t = startT;

	Color instColor;
	double instDensity;
	float lightTrans;
	while (T > EPSILON)
	{
		location = location + rayDirection * m_deltaS;
		current_t += m_deltaS;
		if ((my_smoke_render.IsOutside(location)) || (current_t >= endT))
		{
			color_a += m_backgroundColor * T;
			return color_a;
		}
		else
		{
			instDensity = my_smoke_render.InterpAllValues(location, &instColor, &lightTrans);
			deltaT = exp(-m_kappa*m_deltaS*instDensity);
			T *= deltaT;
			Color light_color(1, 1, 1);
			color_a += instColor * light_color * (1 - deltaT) / m_kappa * T * lightTrans;
		}
	}
	return color_a;
}

void Camera::render()
{
	dx /= 3.0f;
	dy /= 3.0f;
	double this_y = start_y;
	for (int i = 0; i < my_pixel_height; i++)
	{
		double this_x = start_x;
		for (int j = 0; j < my_pixel_width; j++)
		{
			double tmp_x = this_x;
			double tmp_y = this_y;
			Color my_color(0, 0, 0);
			for (int m = 0; m < 3; m++)
			{
				tmp_y += dy;
				for (int n = 0; n < 3; n++)
				{
					tmp_x += dx;
					Color tmp_color;
					Vector3 my_ray_direction;
					my_ray_direction = my_front * my_flength + my_up * tmp_y + my_right * tmp_x;
					Ray my_ray(my_position, my_ray_direction);
					double my_t;
					Primitive* obj = rayTrace(my_ray, tmp_color, 1, 1.0f, my_t);
					my_color += tmp_color;
				}
			}
			screen[i][j] = my_color / 9.0;
			this_x += dx * 3;
		}
		if (i == my_pixel_height / 3)
		{
			std::cout << "1/3 raytracing: done." << endl;
		}
		else if (i == 5*my_pixel_height / 12)
		{
			std::cout << "5/12 raytracing: done." << endl;
			// could be a endless recursion.
			// consider the smoke part: what the process is. and when the process can be ended.
		}
		else if (i == my_pixel_height / 2)
		{
			std::cout << "1/2 raytracing: done." << endl;
		}
		else if (i == 2*my_pixel_height / 3)
		{
			std::cout << "2/3 raytracing: done." << endl;
		}
		this_y += dy * 3;
	}
}