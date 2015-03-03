#include "StdAfx.h"
#include "MVector3.h"                       /**< ����ͷ�ļ� */
#include <math.h>

/** ���������ĳ��� */
inline double Vector3::length()
{
	return ( double )sqrt( x * x + y * y + z * z );
}

/** ��λ��һ���� */
Vector3 Vector3::normalize()
{
	double len = length();                  /**< ������������ */
	if( len == 0 )
		len = 1;
	x = x / len;
	y = y / len;
	z = z / len;

	return *this;
}

/** ��� */
 double Vector3::dotProduct(const Vector3& v)
{
	return ( x * v.x + y * v.y + z * v.z );
}

/** ��� */
Vector3 Vector3::crossProduct(const Vector3& v)
{
	Vector3 vec;

	vec.x = y * v.z - z * v.y;
	vec.y = z * v.x - x * v.z;
	vec.z = x * v.y - y * v.x;
	
	return vec;
}

/** ������ + */
 Vector3 Vector3::operator +(const Vector3& v)
{
	Vector3 vec;
	
	vec.x = x + v.x;
	vec.y = y + v.y;
	vec.z = z + v.z;

	return vec;
}

/** ������ - */
 Vector3 Vector3::operator -(const Vector3& v)
{
	Vector3 vec;
	
	vec.x = x - v.x;
	vec.y = y - v.y;
	vec.z = z - v.z;

	return vec;
}

/** ������ * */
 Vector3 Vector3::operator *(double scale)
{
	x = x * scale;
	y = y * scale;
	z = z * scale;
	
	return *this;
}

/** ������ / */
 Vector3 Vector3::operator /(double scale)
{
	if(scale != 0.0)
	{	
	   x = x / scale;
	   y = y / scale;
	   z = z / scale;
	}
	return *this;
}

/** ���� */
 Vector3 Vector3::operator -()
{
	Vector3 vec( - x,- y, - z);
	return vec;
}
 Vector3 Vector3::operator +=(const Vector3& v)
 {
	 *this = *this + v;
	 return *this;
 }

 Vector3 Vector3::operator = (const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}
bool Vector3::operator == (const Vector3& v)
{
	if (v.x == x && v.y == y && v.z == z)
	{
		return true;
	}
	else{
		return false;
	}
	
}











