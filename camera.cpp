#include "camera.h"

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
}

Primitive* Camera::rayTrace(Ray& ray, Color& pixel_color, int re_depth, double index, double& t)
{
	if (re_depth > TRACINGDEPTH) return 0;

	t = 10000.0f;
	Primitive* object = 0;
	int result;
	for (int i = 0; i < my_image->getNumber(); i++)
	{
		Primitive* tmp_object = my_image->getPrimitive(i);
		int tmp_result = tmp_object->intersect(ray, t);
		if (tmp_result)
		{
			result = tmp_result;
			object = tmp_object;
		}
	}
	// hit nothing
	if (!object) return 0;
	// hit a light
	if (object->isLight())
	{
		pixel_color = object->getMaterial()->getColor();
	}
	// hit a object
	else
	{
		Vector3 V = ray.getDirection();
		Vector3 intersection = ray.getPoint(t);
		Vector3 N = object->getNormal(intersection);
		Color object_color = object->getMaterial()->getColor();
		for (int j = 0; j < my_image->getNumber(); j++)
		{
			Primitive* next_object = my_image->getPrimitive(j);
			if (next_object->isLight())
			{
				Primitive* light = next_object;
				if (light->getType() == Primitive::SPHERE)
				{
					Vector3 L = ((Sphere*)light)->getCenter() - intersection;
					Vector3 R = L - N * 2.0f * L.Dot(N);
					R.Normalize();
					Color light_color = light->getMaterial()->getColor();
					// shadow
					double shadow = 1.0f;
					double tmp_t = L.L2Norm();
					L.Normalize();
					Ray shadow_ray = Ray(intersection + L*EPSILON, L);
					for (int k = 0; k < my_image->getNumber(); k++)
					{
						Primitive* shadow_object = my_image->getPrimitive(k);
						// early determination
						if ((!shadow_object->isLight()) && (shadow_object->intersect(shadow_ray, tmp_t)))
						{
							shadow = 0;
							break;
						}
					}
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
		//reflection
		double reflection = object->getMaterial()->getReflection();
		if ((reflection > 0) && (re_depth < TRACINGDEPTH))
		{
			Vector3 re_R = V - N * V.Dot(N) * 2.0f;
			Color re_color(0, 0, 0);
			double re_t;
			rayTrace(Ray(intersection + re_R * EPSILON, re_R), re_color, re_depth + 1, index, re_t);
			pixel_color += object_color * re_color * reflection;//color blend??
		}
		//refraction
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
			screen[i][j] = my_color/9.0;
			this_x += dx * 3;
		}
		this_y += dy * 3;
	}
}