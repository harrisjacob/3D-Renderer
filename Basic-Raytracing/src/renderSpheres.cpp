#include <vector>
#include "Sphere.h"


#if defined __linux__ || defined __APPLE__
#else
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

#define MAX_RAY_DEPTH 5

float mix(const float &a, const float &b, const float &mix){
	return b*mix + a * (1-mix);
}

//Returns a color for a given pixel and can be called recursively to the maximum depth
Vec3f trace(const Vec3f &rayOrigin, const Vec3f &rayDirection, const std::vector<Sphere> &spheres, const int &depth){

	//Define closest elements and distance
	float closestIntersect = INFINITY;
	const Sphere* closestSphere = NULL;

	//Iterate through spheres to find the closest intersection
	for(const Sphere& oneSphere : spheres){
		float nearIntersect = INFINITY;
		float farIntersect = INFINITY;
		//If there is an intersection with the current sphere
		if(oneSphere.intersect(rayOrigin, rayDirection, nearIntersect, farIntersect)){
			if(nearIntersect < 0 ) nearIntersect = farIntersect; // If the nearIntersect is behind
			//Check and update closest sphere
			if(nearIntersect < closestIntersect){
				closestIntersect = nearIntersect;
				closestSphere = &oneSphere;
			}

		}

	}

	if(!closestSphere) return Vec3f(2); //No intersection occured, set as background color

	Vec3f surfaceColor = 0;		//Color of the ray/surface of the object intersected by the ray
	Vec3f pointOfIntersect = rayOrigin + rayDirection*closestIntersect;	//rayDirection should be passed normalized
	Vec3f normalOfIntersect = pointOfIntersect - closestSphere->center;		//normal at intersection point
	normalOfIntersect.normalize();										//Normalize normal vector


	float bias = 1e-4;				//Bias added to the point being traced
	bool inside = false;			//Is view inside a sphere

	if(rayDirection.dot(normalOfIntersect) > 0){	//Test for inside
		//If ray direction and normal vector are pointing in the same direction (relatively)
		//then the view must be from inside a sphere

		normalOfIntersect = -normalOfIntersect;		//Flip normal
		inside = true;
	}


	if((closestSphere->transparency > 0 || closestSphere->reflection > 0) && depth < MAX_RAY_DEPTH){
		//Only make recursive calls if they are needed and max depth has not been reached

		float facingRatio = -rayDirection.dot(normalOfIntersect); //Ray direction and normal vector should be pointing in opposite directions

		//Gives the effect of the reflection becoming less defined the further the subject is from the point of reflection
		//Change the last argument (mix value) to tweak the effect
		float fresnelEffect = mix(pow(1-facingRatio, 3), 1, 0.1);


		//Compute reflection direction, rayDirection and normal vector should already be normalized
		Vec3f reflectDirection = rayDirection - normalOfIntersect * 2 * rayDirection.dot(normalOfIntersect);
		reflectDirection.normalize();

		Vec3f reflection = trace(pointOfIntersect+normalOfIntersect*bias, reflectDirection, spheres, depth+1);
		//Recursively call trace function to get a reflection value

		Vec3f refraction = 0;

		//If sphere is transparent, a refraction ray (trasmission) needs to be calculated
		if(closestSphere->transparency){

			float ior = 1.1;  //Chosen index of refraction value
			
			//The following is an implementation of refractive equations desribed in a paper written by Bram de Greve
			//Source: https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
			
			float eta = (inside) ? ior :  1/ior; //Greek symbol eta is the ratio of IORs: (IOR_prev_material/IOR_new_material)
			//If already in the sphere, the ratio is flipped

			float cosI = -normalOfIntersect.dot(rayDirection); //cosine of angle of incidence
			
			//See conclusion of above source
			Vec3f refractDirection = rayDirection*eta + normalOfIntersect*(eta*cosI - sqrt(1-(eta*eta*(1-cosI*cosI))));

			refractDirection.normalize();

			//Recursively call trace function to get refraction color influence
			refraction = trace(pointOfIntersect - normalOfIntersect*bias, refractDirection, spheres, depth+1);
		}


		//The result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (reflection*fresnelEffect + refraction * (1-fresnelEffect) * closestSphere->transparency) * closestSphere->surfaceColor;

	}else{
		//If max ray depth has been reached or its a diffuse object (no transparency or reflection), theres no need to raytrace any further

		for(const Sphere& firstSphere: spheres){ 			//Check for light being emitted from other spheres 
			if(firstSphere.emissionColor.x > 0){
				//Its a light source
				Vec3f transmission = 1;
				Vec3f lightDirection = firstSphere.center - pointOfIntersect;
				lightDirection.normalize();


				for(const Sphere& intersectSphere: spheres){
					if(&intersectSphere == &firstSphere) continue;

					float t0, t1; //Don't need these values, just need to fill function intersect parameters

					if(intersectSphere.intersect(pointOfIntersect + normalOfIntersect*bias, lightDirection, t0, t1)){
						transmission = 0;
						break;
					}

				}

				//Add emission color contributions from each of the spheres that emit light
				surfaceColor += closestSphere->surfaceColor * transmission * std::max(float(0), normalOfIntersect.dot(lightDirection)) * firstSphere.emissionColor;

			}

		}

	}

	return surfaceColor + closestSphere->emissionColor;
}




int main(){
	return 0;
}