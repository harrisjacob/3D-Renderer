#include "Model.h"

#include <iostream> //std::cerr
#include <sstream> //std::istringstream
#include <fstream> //std::ifstream
#include <string> //std::c_str


//Constructor
Model::Model(const char *filename) : vertices_(), faces_() {
	std::ifstream in;
	
	//Open the OBJ file
	in.open(filename, std::ifstream::in);
	if(in.fail()) return;

	std::string line;
	while(!in.eof()){
		std::getline(in, line);					//Get a single line from the input stream and read into string line
		std::istringstream iss(line.c_str());	//Construct input stream from line to parse values (converts line to a C style string first)
		char trash;								//Placeholder for reading values not needed
		if(!line.compare(0, 2, "v ")){			//Compare the first two characters (is it a vertex?)
			iss >> trash;						//Trash data type indicator(vertex)
			//Read in a vector
			Vec3f v;
			iss >> v.x >> v.y >> v.z;
			vertices_.push_back(v);
		}else if(!line.compare(0, 2, "f ")){	//Is it a face?
			std::vector<int> f;
			iss>>trash;							//Trash the data type indicator(face)
			int valTrash;						//TODO: Read in texture coordinates and normals (for now trash the vals)
			int idx;						//Trash the slash character delimeters

			//Format "vertexIdx/vertexTextureIdx/vertexNormalIdx ../../.. ../../.."
			while(iss >> idx >> trash >> valTrash >> trash >> valTrash){
				//Note each input stream has 3 vertices so this loop executes 3 times
				idx--;				//Wavefront obj indexing starts at 1
				f.push_back(idx);
			}

			faces_.push_back(f);

		}	

	}

}

//Get the vertex count
int Model::nVerts() const{
	return vertices_.size();
}


//Get the face count
int Model::nFaces() const{
	return faces_.size();
}

//Get a vertex at index idx
Vec3f Model::vert(int idx) const{
	return vertices_[idx];
}

//Get a face at index idx
std::vector<int> Model::face(int idx) const{
	return faces_[idx];
}

void Model::debug(){
	std::cerr << "#v: " << nVerts() << " #f " << nFaces() << "\n";
}