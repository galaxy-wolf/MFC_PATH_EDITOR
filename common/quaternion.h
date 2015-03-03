#ifndef QUATERNION_H
#define QUATERNION_H

#include "util.h"
#include "math_constants.h"
#include "vector3.h"
#include "matrix.h" // for conversion to matrix object

#include <math.h>


class quaternion
{
private:
public:
	float x, y, z, w;
	quaternion()
	{
	}
	// removed normalization of axis and xyzw from set
	quaternion(const vector3 &axis, float radians)
	{
		set(axis, radians);
	}
	void set(const vector3 &axis, float radians)
	{
		radians *= .5f;
		w = cosf(radians);
		float sr = sinf(radians);
		x = axis.v[0] * sr;
		y = axis.v[1] * sr;
		z = axis.v[2] * sr;
	}
	quaternion(const float *a, float radians)
	{
		set(a, radians);
	}
	void set(const float *a, float radians)
	{
		radians *= .5f;
		w = cosf(radians);
		float sr = sinf(radians);
		x = a[0] * sr;
		y = a[1] * sr;
		z = a[2] * sr;
	}
	quaternion(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w)
	{
	}
	void set(const float _x, const float _y, const float _z, const float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	// from 4x4 matrix
	quaternion(const float *mat)
	{
		set(mat);
	}
	void set(const float *mat)
	{
		w = 1.0f + mat[0] + mat[5] + mat[10];
		if (w <= 0)
		{
			identity();
		}
		else
		{
			w = sqrt(w) * .5f;
			float invW4 = .25f / w;
			x = (mat[9] - mat[6]) * invW4;
			y = (mat[2] - mat[8]) * invW4;
			z = (mat[4] - mat[1]) * invW4;
		}
	}

	// multiplication identity (addition identity is 0, <0, 0, 0,>)
	quaternion& identity()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
		return *this;
	}

	quaternion operator + (const quaternion &p) const
	{
		return quaternion(x + p.x, y + p.y, z + p.z, w + p.w);
	}
	quaternion& operator += (const quaternion &p)
	{
		x += p.x;
		y += p.y;
		z += p.z;
		w += p.w;
		return *this;
	}

	quaternion operator * (const quaternion &param) const
	{
		quaternion qp(*this);
		qp *= param;

		return qp;
	}
	quaternion& operator *= (const quaternion &param)
	{
		float vp[3];

		vp[0] = w * param.x + x * param.w + y * param.z - z * param.y;
		vp[1] = w * param.y - x * param.z + y * param.w + z * param.x;
		vp[2] = w * param.z + x * param.y - y * param.x + z * param.w;

		w = w * param.w - (x * param.x + y * param.y + z * param.z);

		x = vp[0];
		y = vp[1];
		z = vp[2];

		return *this;
	}

	// these could be confusing...
	/*quaternion operator * (const float p) const
	{
		return quaternion(p * x, p * y, p * z, p * w);
	}*/
	quaternion& operator *= (const float p)
	{
		x *= p;
		y *= p;
		z *= p;
		w *= p;
		return *this;
	}

	vector3 operator * (const vector3 &v) const
	{
		vector3 uv, uuv;
		vector3 qvec(x, y, z);
		uv = Cross(qvec, v);
		uuv = Cross(qvec, uv);
		uv *= 2 * w;
		uuv *= 2;
		return v + uv + uuv;
	}

	void matrix(float *m) const
	{
		float x2, y2, z2, xx, yy, zz, xy, yz, xz, wx, wy, wz;
		x2 = x + x;
		y2 = y + y;
		z2 = z + z;
		xx = x * x2;
		yy = y * y2;
		zz = z * z2;
		xy = x * y2;
		yz = y * z2;
		xz = z * x2;
		wx = w * x2;
		wy = w * y2;
		wz = w * z2;
		m[0] = 1 - (yy + zz);
		m[1] = xy + wz;
		m[2] = xz - wy;
		m[3] = 0;
		m[4] = xy - wz;
		m[5] = 1 - (xx + zz);
		m[6] = yz + wx;
		m[7] = 0;
		m[8] = xz + wy;
		m[9] = yz - wx;
		m[10] = 1 - (xx + yy);
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = 1;
	}
	// same as above, but returns a matrix object
	::matrix matrix() const
	{
		::matrix rval;
		matrix(rval.m);
		return rval;
	}
	void matrix4x3(float *m) const
	{
		float x2, y2, z2, xx, yy, zz, xy, yz, xz, wx, wy, wz;
		x2 = x + x;
		y2 = y + y;
		z2 = z + z;
		xx = x * x2;
		yy = y * y2;
		zz = z * z2;
		xy = x * y2;
		yz = y * z2;
		xz = z * x2;
		wx = w * x2;
		wy = w * y2;
		wz = w * z2;
		m[0] = 1 - (yy + zz);
		m[1] = xy + wz;
		m[2] = xz - wy;
		m[3] = 0;
		m[4] = xy - wz;
		m[5] = 1 - (xx + zz);
		m[6] = yz + wx;
		m[7] = 0;
		m[8] = xz + wy;
		m[9] = yz - wx;
		m[10] = 1 - (xx + yy);
		m[11] = 0;
//		m[12] = 0;
//		m[13] = 0;
//		m[14] = 0;
//		m[15] = 1;
	}
	quaternion slerp(const quaternion &q, const float c) const
	{
		float omega, cosfomega, sinfomega, k1, k2;
		quaternion q2, q3;
		cosfomega = x * q.x + y * q.y + z * q.z + w * q.w;
		if (cosfomega < 0)
		{
			cosfomega = -cosfomega;
			q2.x = -q.x;
			q2.y = -q.y;
			q2.z = -q.z;
			q2.w = -q.w;
		}
		else
			q2 = q;

		if (1 - cosfomega > .000001f)
		{
			omega = acosf(cosfomega);
			sinfomega = sinf(omega);
			k1 = sinf((1 - c) * omega) / sinfomega;
			k2 = sinf(c * omega) / sinfomega;
		}
		else
		{
			k1 = 1 - c;
			k2 = c;
		}

		q3.x = x * k1 + q2.x * k2;
		q3.y = y * k1 + q2.y * k2;
		q3.z = z * k1 + q2.z * k2;
		q3.w = w * k1 + q2.w * k2;

		return q3;
	}

	// this is the inverse if magnitude == 1 (should be the case when using rotations)
	quaternion& conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	quaternion& inverse()
	{
		conjugate();
		float invmag2 = 1.0f / magnitude2();
		x *= invmag2;
		y *= invmag2;
		z *= invmag2;
		w *= invmag2;
		return *this;
	}

	float magnitude() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	float magnitude2() const
	{
		return x * x + y * y + z * z + w * w;
	}

	quaternion& normalize()
	{
		float mag = magnitude();
		if (mag < .000001f)
			return *this;
		float invmag = 1.0f / mag;
		x *= invmag;
		y *= invmag;
		z *= invmag;
		w *= invmag;
		return *this;
	}

	quaternion& scale(const float s)
	{
		w *= s;
		float s1 = sqrt(1.0f - w * w);
		float s2 = sqrt(x * x + y * y + z * z);
		float s3 = s1 / s2;
		if (s2 < .00001f)
		{
			x = 0;
			y = 0;
			z = 0;
		}
		else
		{
			x *= s3;
			y *= s3;
			z *= s3;
		}
		return *this;
	}

	bool operator == (const quaternion &p) const
	{
		return (x == p.x && y == p.y && z == p.z && w == p.w);
	}
	bool operator != (const quaternion &p) const
	{
		return (x != p.x || y != p.y || z != p.z || w != p.w);
	}
};

#endif
