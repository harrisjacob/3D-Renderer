#ifndef __VEC2_H__
#define __VEC2_H__

#include <ostream>
#include <cmath>

template <typename T>
class Vec2;

template <typename T>
std::ostream& operator<<(std::ostream&, const Vec2<T> &);

template <typename T>
class Vec2 {

public:
	T x_, y_;

	Vec2();
	Vec2(T x);
	Vec2(T x, T y);

	Vec2<T>& normalize();

	Vec2<T> operator*(const T&) const;
	Vec2<T> operator*(const Vec2<T> &) const;
	T dot(const Vec2<T> &) const;
	Vec2<T> operator- (const Vec2<T> &) const;
	Vec2<T> operator+ (const Vec2<T> &) const;
	Vec2<T>& operator+= (const Vec2<T> &);
	Vec2<T>& operator-= (const Vec2<T> &);
	Vec2<T> operator- () const;

	T length2() const;
	T length();

	friend std::ostream& operator<< <>(std::ostream&, const Vec2<T> &);


};

//Default constructor
template <typename T>
Vec2<T>::Vec2(): x_(T(0)), y_(T(0)){};

//Single value constructor
template <typename T>
Vec2<T>::Vec2(T x): x_(T(x)), y_(T(x)){};

//Two value constructor
template <typename T>
Vec2<T>::Vec2(T x, T y): x_(T(x)), y_(T(y)){};

//Normalize vector
template <typename T>
Vec2<T>& Vec2<T>::normalize(){
	T nor2 = length2();
	if(nor2){
		T invNor = 1/sqrt(nor2);
		x_*=invNor;
		y_*=invNor;
	}
	return *this;
}

//Overload multiplcation by scalar
template <typename T>
Vec2<T> Vec2<T>::operator*(const T &f) const{
	return Vec2<T>(x_*f, y_*f);
}

//Overload multiplication by vector
template <typename T>
Vec2<T> Vec2<T>::operator*(const Vec2<T> &v) const{
	return Vec2<T>(x_*v.x_, y_*v.y_);
}

//Dot product of two vectores
template <typename T>
T Vec2<T>::dot(const Vec2<T> & v) const{
	return x_*v.x_+y_*v.y_;
}

//Overload subtraction operator
template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T> &v) const{
	return Vec2<T>(x_-v.x_, y_-v.y_);
}

//Overload addition operator
template <typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T> &v) const{
	return Vec2<T>(x_+v.x_, y_+v.y_);
}

//Overload addition assignment operator
template <typename T>
Vec2<T>& Vec2<T>::operator+=(const Vec2<T> &v){
	x_ += v.x_;
	y_ += v.y_;
	return *this;
}

//Overload subtraction assignment operator
template <typename T>
Vec2<T>& Vec2<T>::operator-=(const Vec2<T> &v){
	x_ -= v.x_;
	y_ -= v.y_;
	return *this;
}

//Squared length of the vector
template <typename T>
T Vec2<T>::length2() const{
	return x_*x_+y_*y_;
}

//Length of the vector
template <typename T>
T Vec2<T>::length() {
	return sqrt(length2());
}

//Print vector to an output stream (debugging)
template <typename T>
std::ostream& operator<<(std::ostream & os, const Vec2<T> &v){
	os << "[" << v.x_ << "," << v.y_ << "]";
	return os;
}

#endif //__VEC2_H__