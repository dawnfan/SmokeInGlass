#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "ray.h"
#include "image.h"
#include "smoke.h"

#define TRACINGDEPTH 5

class Camera
{
private:
	Vector3 my_position;
	Vector3 my_front;
	Vector3 my_up;
	Vector3 my_right;
	Vector3 my_screen_center;
	double my_flength;
	double my_screen_width, my_screen_height;
	double dx, dy;
	double start_x, start_y;
	int my_pixel_width, my_pixel_height;
	Image* my_image;
	double m_deltaS;
	double m_kappa;
	Color m_backgroundColor;
	std::vector<Sphere*> my_smoke_spheres;
	std::vector<Sphere*> my_lights;
	Smoke my_smoke_render;

	int rayHitTest(Ray ray, double &t, Primitive* &obj_out, bool isShadow = false);
	Color getBaseColor(Ray ray, double t, Primitive* object);
public:
	Camera();
	~Camera() { delete my_image; }
	Image* getImage() { return my_image; }
	Primitive* rayTrace(Ray& ray, Color& pixel_color, int re_depth, double index, double& t);
	void render();
	Color screen[PIXELHEIGHT][PIXELWIDTH];
	// smoke rendering
	void GenerateSpheres();
	void GenerateSmoke();
	Color rayMarch(Vector3& rayDirection, double startT, double endT);
};

#endif	// CAMERA_H