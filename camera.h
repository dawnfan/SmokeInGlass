#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "image.h"

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

	int rayHitTest(Ray ray, double &t, Primitive* &obj_out, bool isShadow = false);
	Color getBaseColor(Ray ray, double t, Primitive* object);
public:
	Camera();
	~Camera() { delete my_image; }
	Image* getImage() { return my_image; }
	Primitive* rayTrace(Ray& ray, Color& pixel_color, int re_depth, double index, double& t);
	void render();
	Color screen[PIXELHEIGHT][PIXELWIDTH];
};

#endif	// CAMERA_H