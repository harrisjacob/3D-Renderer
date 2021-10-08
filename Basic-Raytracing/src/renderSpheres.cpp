#include <vector>
#include <fstream>
#include "Sphere.h"


#if defined __linux__ || defined __APPLE__
#else
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

#define MAX_RAY_DEPTH 5

//Used by the fresnelEffect caluclation to mix the reflective and refractive values
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


void render(const std::vector<Sphere>& spheres){

	unsigned width = 640, height = 480;

	Vec3f *image = new Vec3f[width*height], *pixel = image;
	//Image is a dynamically allocate array of RGB values, pixel and image points to the first vector in array

	//The following is an implementation of a camera ray generation provided by scratchapixel.com
	//Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays

	float invWidth = 1/float(width), invHeight = 1/float(height);
	float fov = 30, aspectRatio = width / float(height);

	float angle = tan(M_PI * 0.5 * fov / 180);
	//tan is evaulated in radians hence the pi/180, the FOV must be split in half because it is centered at the middle of the screen

	//Generate rayDirection for each pixel in image
	for(unsigned y=0; y<height; y++){
		for(unsigned x=0; x<width; x++, pixel++){ //Iterate to the next pixel each time
			
			float xComponent = (2*((x+0.5)*invWidth) - 1) * angle * aspectRatio;
			float yComponent = (1 - 2*((y+0.5)*invHeight)) * angle;

			//For each pixel x, 0.5 is added to center the value horizontally on the pixel
			//Dividing by the width (multiplying by invWidth) gives the percentage of horizontal placement, far left being 0 and far right being 1
			//This value is in the range [0,1], but the canvas should be in the range [-1,1] so multiply by two and shift left
			//Multiplying by aspectRation unsquashes the pixels making them square relative to the pixel height
			//Multiplying by the angle stretches or squashes the images based on input angle
			//The yComponent is (1 - 2 * ...) ... because pixels above the camera should have positive values, and those below should have negative values

			Vec3f rayDirection(xComponent, yComponent, -1);
			//The image canvas is 1 unit away from the camera in camera space, and the camera is align along the negative z-axis
			rayDirection.normalize();

			*pixel = trace(Vec3f(0), rayDirection, spheres, 0);

		}
	}

	//Save result to PPM image

	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	//std::ios::out specfied that the file is open for writing, std::ios::binary means operations are performed in binary mode rather than text
	//P6 is a binary encoding (see P6 below)

	//P6 is a magic number used by PPM files, followed by whitespace separated width height, followed by the maximum color value
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for(unsigned i=0; i< width*height; i++){
		ofs << 	(unsigned char)(std::min(float(1), image[i].x) * 255) <<
				(unsigned char)(std::min(float(1), image[i].y) * 255) <<
				(unsigned char)(std::min(float(1), image[i].z) * 255);
		//Each sample is represented in 1 byte pur binary, hence unsigned char
	}

	ofs.close();
	delete[] image;	

}


int main(){
	return 0;
}