#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "stdafx.h"

/** ������ */
class Vector3
{
public:
	/** ���캯�� */
	Vector3( void )  { x = 0.0; y = 0.0; z = 0.0; }
	Vector3( double xx, double yy, double zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}
	Vector3(const Vector3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}
    
	/** ��Ա���� */
	inline double length();                       /**< ������������ */
	Vector3 normalize();                         /**< ��λ������ */
	double dotProduct(const Vector3& v);          /**< ������ */
	Vector3 crossProduct(const Vector3& v);      /**< ������ */

	/** ���ز����� */
	Vector3 operator + (const Vector3& v);
	Vector3 operator - (const Vector3& v);
	Vector3 operator * (double scale);
	Vector3 operator / (double scale);
	Vector3 operator - ();
	Vector3 operator = (const Vector3& v);
	Vector3 operator += (const Vector3& v);
	bool operator == (const Vector3& v);
	
public:
	  double x,y,z;

};

/** ��ά������ */
/*class Vector2
{
public:
	/** ���캯�� */
	/*Vector2(float xx = 0.0,float yy = 0.0) { x = xx; y = yy;  }
	Vector2(const Vector2& v)
	{
		x = v.x;
		y = v.y;
	}

public:
	float x,y;
};
*/

#endif //__VECTOR_H__