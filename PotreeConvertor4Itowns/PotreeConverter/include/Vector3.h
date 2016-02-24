
#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>
#include <iostream>

using std::ostream;
#ifndef _MSC_VER
using std::max;
#endif

namespace Potree{

template<class T>
class Vector3{

public:
	T x = 0;
	T y = 0;
	T z = 0;

	Vector3() = default;

	Vector3(T x, T y, T z){
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(T value){
		this->x = value;
		this->y = value;
		this->z = value;
	}

	Vector3(const Vector3<T> &other)
		:x(other.x), y(other.y), z(other.z)
	{
	}

	~Vector3() = default;


	T length(){
		return sqrt(x*x + y*y + z*z);
	}

	T squaredLength(){
		return x*x + y*y + z*z;
	}
    
    Vector3<T> squaredThis() {
        return Vector3<T>(x*x, y*y, z*z);
    }
    
    
    
    Vector3<T> multipleComponents( Vector3<T>p){
        return Vector3<T>(x*p.x, y*p.y, z*p.z);
    }

	T distanceTo(Vector3<T> p) const{
		return ((*this) - p).length();
	}

	T squaredDistanceTo(const Vector3<T> &p) const{
		return ((*this) - p).squaredLength();
	}

	T maxValue(){
		return max(x, max(y,z));
	}
    
    T dot(const Vector3<T>& p) {
        return x*p.x + y*p.y + z*p.z;
    }
    
    Vector3<T> cross(const Vector3<T> &p){
        return Vector3(y*p.z - z*p.y, z*p.x -x*p.z, x*p.y - y*p.x);
    }

	Vector3<T> operator-(const Vector3<T>& right) const {
		return Vector3<T>(x - right.x, y - right.y, z - right.z);
	}

	Vector3<T> operator+(const Vector3<T>& right) const {
		return Vector3<T>(x + right.x, y + right.y, z + right.z);
	}

	Vector3<T> operator+(const T right) const {
		return Vector3<T>(x + right, y + right, z + right);
	}

	Vector3<T> operator/(const T &a) const{
		return Vector3<T>(x / a, y / a, z / a);
	}
    
    void normalize(){
        T magnitude = sqrt((x*x) + (y*y) + z*z);
        if(magnitude != 0){
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }
    
    void divideByScalar(const T &s){
        if(s != 0){
            x/= s;
            y/= s;
            z/= s;
        }
    }
    
    void multiplyByScalar(const T &s){
        x *= s;
        y *= s;
        z *= s;
    }
    
    void add(const Vector3<T> &p){
        x += p.x;
        y += p.y;
        z += p.z;
    }
    
    void sub(const Vector3<T> &p){
        x -= p.x;
        y -= p.y;
        z -= p.z;
    }

	friend ostream &operator<<( ostream &output,  const Vector3<T> &value ){
		output << "[" << value.x << ", " << value.y << ", " << value.z << "]" ;
		return output;            
	}
};

}

#endif
