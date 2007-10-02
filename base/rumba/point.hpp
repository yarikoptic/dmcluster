#ifndef RUMBA_POINT_H
#define RUMBA_POINT_H

#include <iostream>
#include <cmath>


namespace RUMBA
{
template<class T>
class Point
{
public:
	inline T& x() { return Rep[0]; }
	inline T& y() { return Rep[1]; }
	inline T& z() { return Rep[2]; }
	inline T& t() { return Rep[3]; }

	inline const T& x() const { return Rep[0]; }
	inline const T& y() const { return Rep[1]; }
	inline const T& z() const { return Rep[2]; }
	inline const T& t() const { return Rep[3]; }

	inline T& operator[](int i) { return Rep[i]; }
	inline const T& operator[](int i) const { return Rep[i]; }

	explicit Point ( T a=T(0), T b=T(0), T c=T(0), T d=T(0) ) 
 	{
		Rep[0] = a; Rep[1] = b; Rep[2] = c; Rep[3] = d;
	}

	Point (const Point& p)
	{
		Rep[0]=p.Rep[0];
		Rep[1]=p.Rep[1];
		Rep[2]=p.Rep[2];
		Rep[3]=p.Rep[3];
	}

	Point& operator=(const Point& p)
	{
		if (&p != this)
		{
			Rep[0]=p.Rep[0];
			Rep[1]=p.Rep[1];
			Rep[2]=p.Rep[2];
			Rep[3]=p.Rep[3];
		}
		return *this;
	}
	~Point(){}

        /** Returns norm (distance to 0 in other words)
         * @return norm
         */
        double norm ()
            {
                return std::sqrt((double)inner_product(*this, *this));
            }

private:
	T Rep[4];
};



template<class TYPE>
bool operator == (const RUMBA::Point<TYPE>& left, const RUMBA::Point<TYPE>& right)
{
	return
		left.x() == right.x() &&
		left.y() == right.y() &&
		left.z() == right.z() &&
		left.t() == right.t();
}

template<class TYPE>
bool operator != (const RUMBA::Point<TYPE>& left, const RUMBA::Point<TYPE>& right)
{
	return !(left==right);
}


template<class T>
bool operator<(const RUMBA::Point<T>& left, const RUMBA::Point<T>& right)
{
	if (
			(left.x() < right.x()) ||
			(left.x()==right.x() && left.y() < right.y()) ||
			(left.x()==right.x() && left.y()==right.y() && left.z() < right.z()) ||
			(left.x()==right.x() && left.y()==right.y() && left.z()==right.z() && left.t()<right.t() )
			)
		return true;
	else
		return false;
}

template<class T>
bool operator<=(const RUMBA::Point<T>& left, const RUMBA::Point<T>& right)
{
	return left < right || left == right;
}

template<class T>
bool operator>(const RUMBA::Point<T>& left, const RUMBA::Point<T>& right)
{
	return !(left <= right);
}

template<class T>
bool operator>=(const RUMBA::Point<T>& left, const RUMBA::Point<T>& right)
{
	return !(left < right);
}

template<class T>
T inner_product(const RUMBA::Point<T>& a, const RUMBA::Point<T>& b)
{
	return a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.t() * b.t();
}


template <class T>
RUMBA::Point<T> operator*(const Point<T>& left, const Point<T>& right)
{
	return Point<T> ( left[0]*right[0], left[1]*right[1], left[2]*right[2],
                          left[3]*right[3] );
}

template <class T>
RUMBA::Point<T> operator*(const Point<T>& left, const T right)
{
	return Point<T> ( left[0]*right, left[1]*right, left[2]*right,
                          left[3]*right );
}

template <class T>
RUMBA::Point<T> operator*(const T left, const Point<T>& right)
{
    return (right * left);
}



template <class T>
RUMBA::Point<T> operator/(const Point<T>& left, const Point<T>& right)
{
	return Point<T> ( left[0]/right[0], left[1]/right[1], left[2]/right[2],
                          left[3]/right[3] );
}

template <class T>
RUMBA::Point<T> operator/(const Point<T>& left, const T right)
{
	return Point<T> ( left[0]/right, left[1]/right, left[2]/right,
                          left[3]/right );
}



template <class T>
RUMBA::Point<T> operator+(const Point<T>& left, const Point<T>& right)
{
	return Point<T> ( left[0]+right[0], left[1]+right[1], left[2]+right[2],
			left[3] + right[3] );
}

template <class T>
RUMBA::Point<T> operator+(const Point<T>& left, const T right)
{
	return Point<T> ( left[0]+right, left[1]+right, left[2]+right,
			left[3] + right );
}

template <class T>
RUMBA::Point<T> operator+(const T left, const Point<T>& right)
{
    return (right + left);
}



template <class T>
RUMBA::Point<T> operator-(const Point<T>& left, const Point<T>& right)
{
	return Point<T> ( left[0]-right[0], left[1]-right[1], left[2]-right[2],
                          left[3]-right[3] );
}

template <class T>
RUMBA::Point<T> operator-(const Point<T>& left, const T right)
{
	return Point<T> ( left[0]-right, left[1]-right, left[2]-right,
                          left[3]-right );
}

template <class T>
RUMBA::Point<T> operator-(const T left, const Point<T>& right)
{
    return Point<T> ( left-right[0], left-right[1], left-right[2],
                      left-right[3] );
}



template <class T>
std::ostream& operator<<(std::ostream& o, const Point<T>& p)
{
	return o << "(" << p.x() << "," << p.y() << "," << p.z() << "," << p.t() << ")";
}

typedef Point<int> intPoint;
typedef Point<double> doublePoint;

template <class T> Point<T> squash(const Point<T>& p)
{
	return Point<T> (p.x(),p.y(),p.z(),1);
}

} // namespace RUMBA

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
