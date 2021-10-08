#include "Sphere.h"

Sphere::Sphere(
	const Vec3f &_center, 
	const float &_radius, 
	const Vec3f &_surfaceColor, 
	const float &_reflection, 
	const float &_transparency, 
	const Vec3f &_emissionColor
	) :
	center(_center), radius(_radius), radius2(_radius*_radius), surfaceColor(_surfaceColor),
	emissionColor(_emissionColor), transparency(_transparency), reflection(_reflection)
	{}


bool Sphere::intersect(const Vec3f &rayOrigin, const Vec3f &rayDirection, float &near, float &far) const {

	Vec3f eyeToCenter = center - rayOrigin;							//Vector from the eye position to the center of the given sphere
	float eyeProjDir = eyeToCenter.dot(rayDirection);				//Component of the eyeToCenter vector in the direction of the rayDirection
	if(eyeProjDir < 0) return false;								//If eyeProjDir is less than 0, view is looking away from sphere

	float distFromCenter2 = eyeToCenter.dot(eyeToCenter) - eyeProjDir*eyeProjDir;	//Calculate distance from center using a^2 = c^2-b^2
	if(distFromCenter2 > radius2) return false;										//If dist is greater than radius, no intersection
																					//Note for a, b >=0,  a>b iff a^2 > b^2

	float distToEdge = sqrt(radius2 - distFromCenter2);				//Calculate distance from edge of circle using a = sqrt(c^2-b^2)
	near = eyeProjDir - distToEdge;									//Near intersection is smaller than eyeProjDir in the direction of rayDirection				
	far = eyeProjDir + distToEdge;									//Far intersection is larger than eyeProjDir in the direction of rayDirection

	return true;
}