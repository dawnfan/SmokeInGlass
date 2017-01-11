#ifndef IMAGE_H
#define IMAGE_H

#include "basics.h"
#include "ray.h"

#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive

class Material
{
private:
	Color my_color;
	double my_reflection;
	double my_diffusion;
	double my_specular;
	double my_refraction;
	double my_refr_index;
public:
	Material() : my_color(Color(0.2f, 0.2f, 0.2f)), my_reflection(0), 
		my_diffusion(0.2f), my_specular(0.8f), my_refr_index(1.5) {};
	void setColor(Color& color) { my_color = color; }
	void setReflection(double reflection) { my_reflection = reflection; }
	void setDiffusion(double diffusion) { my_diffusion = diffusion; }
	void setSpecular(double specular) { my_specular = specular; }
	void setRefraction(double refraction) { my_refraction = refraction; }
	void setRefrIndex(double refr_index) { my_refr_index = refr_index; }
	Color getColor() { return my_color; }
	double getReflection() { return my_reflection; }
	double getDiffusion() { return my_diffusion; }
	double getSpecular() { return 1.0f - my_diffusion; }
	double getRefraction() { return my_refraction; }
	double getRefrIndex() { return my_refr_index; }
};

class Primitive
{
protected:
	Material my_material;
	char* my_name;
	bool my_light;
	// for smoke rendering
	bool my_smoke;
	float my_Density;
public:
	enum
	{
		SPHERE = 1,
		PLANE
	};
	Primitive() : my_name(0), my_light(false) {};
	Material* getMaterial() { return &my_material; }
	void setMaterial(Material& material) { my_material = material; }
	bool isLight() { return my_light; }
	virtual void setLight(bool light) { my_light = light; }
	virtual int getType() = 0;
	virtual int intersect(Ray& ray, double& t) = 0;
	virtual Vector3 getNormal(Vector3& position) = 0;
	// for smoke rendering
	bool isSmoke() { return my_smoke; }
	float getDensity() { return my_Density; }
};

class Sphere : public Primitive
{
private:
	Vector3 my_center;
	double my_Radius;
public:
	Sphere(Vector3& center, double radius) : my_center(center), my_Radius(radius) {};
	Vector3& getCenter() { return my_center; }
	double getRadius() { return my_Radius; }
	int getType() { return SPHERE; }
	virtual int intersect(Ray& ray, double& t) override;
	Vector3 getNormal(Vector3& position) { return (position - my_center)/my_Radius; }
};

class Plane : public Primitive
{
private:
	Vector3 my_normal;
	double my_d;
public:
	Plane(Vector3& normal, double d) : my_normal(normal), my_d(d) {};
	Vector3& getNormal() { return my_normal; }
	double getD() { return my_d; }
	int getType() { return PLANE; }
	virtual int intersect(Ray& ray, double& t) override;
	Vector3 getNormal(Vector3& position) { return my_normal; }
};

class Image
{
private:
	int number;
	Primitive** my_primitive;
public:
	Image() { init(); }
	~Image() { delete my_primitive; }
	void init();
	int getNumber() { return number; }
	Primitive* getPrimitive(int index) { return my_primitive[index]; }
};


#endif	// IMAGE_H