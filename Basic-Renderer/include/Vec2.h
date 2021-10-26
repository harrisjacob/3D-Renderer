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

Vec2<T>& Vec2<T>::normalize(){
	
}


#endif //__VEC2_H__