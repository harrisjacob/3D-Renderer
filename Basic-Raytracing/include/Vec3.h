
template <typename T>
class Vec3;

template <typename T>
std::ostream& operator<<(std::ostream&, const Vec3<T> &);


template <typename T>
class Vec3 {

public:
	T x, y, z;

	Vec3();
	Vec3(T _x);
	Vec3(T _x, T _y, T _z);

	Vec3<T>& normalize();

	Vec3<T> operator* (const T &) const;
	Vec3<T> operator* (const Vec3<T> &) const;
	T dot(const Vec3<T> &) const;
	Vec3<T> operator- (const Vec3<T> &) const;
	Vec3<T> operator+ (const Vec3<T> &) const;
	Vec3<T>& operator+= (const Vec3<T> &) const;
	Vec3<T>& operator-= (const Vec3<T> &) const;
	Vec3<T> operator- () const;

	T length2() const;
	T length();

	friend std::ostream& operator<< <>(std::ostream&, const Vec3<T> &);

};