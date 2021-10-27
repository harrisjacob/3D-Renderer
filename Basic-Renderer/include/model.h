#ifndef __MODEL_H__
#define __MODEL_H__

#include "Geometry.h"
#include <vector>


class Model {
private:
	std::vector<Vec3f> vertices_;
	std::vector<std::vector<int>> faces_;
public:
	Model(const char*);
	int nVerts() const;
	int nFaces() const;
	Vec3f vert(int) const;
	std::vector<int> face(int) const;
	void debug();
};

#endif // __MODEL_H__