#include <ostream>

#include "Vec3.h"

//Single value constructor
template <typename T>
Vec3<T>::Vec3(T _x): x(_x), y(_x), z(_x){};

//Three value constructor
template <typename T>
Vec3<T>::Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z){};

//Vector normalization
template <typename T>
Vec3<T>& Vec3<T>::normalize(){
	T nor2 = length2();
	if(nor2){
		T invNor = 1/sqrt(nor2);
		x*=invNor;
		y*=invNor;
		z*=invNor;
	}
	return *this;
}

//Overload multiplication by scalar
template <typename T>
Vec3<T> Vec3<T>::operator* (const T &f) const{
	return Vec3<T>(x*f, y*f, z*f);
}

//Overload multiplication by a vector
template <typename T>
Vec3<T> Vec3<T>::operator* (const Vec3<T> &v) const{
	return Vec3<T>(x*v.x, y*v.y, z*v.z);
}

//Dot product
template <typename T>
T Vec3<T>::dot(const Vec3<T> &v) const{
	return x*v.x+y*v.y+z*v.z;
}

//Overload subtraction by vector
template <typename T>
Vec3<T> Vec3<T>::operator- (const Vec3<T> &v) const{
	return Vec3<T>(x-v.x, y-v.y, z-v.z);
}

//Overload addition by vector
template <typename T>
Vec3<T> Vec3<T>::operator+ (const Vec3<T> &v) const{
	return Vec3<T>(x+v.x. y+v.y, z+v.z);
}

//Overload addition assignment by vector
template <typename T>
Vec3<T>& Vec3<T>::operator+= (const Vec3<T> &v) const{
	x+=v.x;
	y+=v.y;
	z+=v.z;
	return *this;
}

//Overload subtraction assignment by vector
template <typename T>
Vec3<T>& Vec3<T>::operator-= (const Vec3<T> &v) const{
	x-=v.x;
	y-=v.y;
	z-=v.z;
	return *this;
}

//Overload negation
template <typename T>
Vec3<T> Vec3<T>::operator- () const{
	return Vec3(-x, -y, -z);
}

//Squared length of vector
template <typename T>
T Vec3<T>::length2() const{
	return x*x+y*y+z*z;
}

//Length of the vector
template <typename T>
T Vec3<T>::length(){
	return sqrt(length2());
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T> &v){
	os << "[" << v.x << "," << v.y << "," << v.z << "]";
	return os;
}

