#ifndef	__BRESENHAM_H__
#define __BRESENHAM_H__


#include "TGAImage.h"
#include "Geometry.h"
#include "Model.h"

#include <cmath>

void line(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor& color){
	bool steep = false;							
	if(std::abs(x0-x1) < std::abs(y0-y1)){		//Is change in y greater than change in x (is slope > 1?)
		std::swap(x0, y0);						//Swap x and y coordinates so the change in x is always greater
		std::swap(x1, y1);
		steep = true;
	}

	if(x0>x1){									//Make sure x0 is always less than x1
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	//See the scale funciton in the TGAImage for explanation on errors variables or variables ending in 2
	int dx = x1-x0;
	int dy = y1-y0;

	int derror2 = std::abs(dy)*2;
	int error2 = 0;

	int y = y0;
	const int yIncr = y1>y0 ? 1: -1;

	//Duplicate code to prevent rechecking bool each iteration (reduction of branching)
	if(steep){
		for(int x=x0; x<=x1; x++){
			image.set(y, x, color);		//We swapped the coordinates earlier so we need to swap back
			error2 += derror2;
			if(error2 > dx) {
				y+=yIncr;
				error2 -= dx*2;
			}	
		}


	}else{
		for(int x=x0; x<=x1; x++){
			image.set(x, y, color);
			error2 += derror2;
			if(error2 > dx) {
				y+=yIncr;
				error2 -= dx*2;
			}	
		}
	}

}

void Bresenham(const Model* model){
	
	const int width = 1000;
	const int height = 1000;


	const TGAColor white = TGAColor(255, 255, 255);

	TGAImage image(width, height, TGAImage::RGB);
	for (int i=0; i<model->nFaces(); i++) { 
    	std::vector<int> face = model->face(i); 
    	for (int j=0; j<3; j++) { 
	        Vec3f v0 = model->vert(face[j]); 
	        Vec3f v1 = model->vert(face[(j+1)%3]); 
	        int x0 = (v0.x+1.)*width/2.; 
	        int y0 = (v0.y+1.)*height/2.; 
	        int x1 = (v1.x+1.)*width/2.; 
	        int y1 = (v1.y+1.)*height/2.; 
	        line(x0, y0, x1, y1, image, white); 
	    } 
	}


	image.write_tga_file("output.tga");
}

#endif //__BRESENHAM_H__