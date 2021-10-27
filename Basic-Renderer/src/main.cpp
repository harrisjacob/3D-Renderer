#include "Model.h"
#include "Bresenham.h"

int main(int argc, char** argv){

	Model *model = NULL;
	if(argc==2){
		model = new Model(argv[1]);
	}else{
		model = new Model("./obj/CoronaCap.obj");
	}

	Bresenham(model);

	delete model;
	return 0;
}