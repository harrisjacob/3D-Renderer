#include "Vec3.h"

class Sphere{
public:
	Vec3f center;						//position of sphere
	float radius, radius2;				//sphere radius and radius^2
	Vec3f surfaceColor, emissionColor;	//surface color and emission (light)
	float transparency, reflection;		//sufrace transparency and reflectivity

	Sphere(const Vec3f &, const float &, const Vec3f &, const float & =0, const float & =0, const Vec3f & =0);

	bool intersect(const Vec3f &, const Vec3f &, float &, float &) const;

};