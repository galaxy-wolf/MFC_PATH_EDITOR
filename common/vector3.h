#ifndef VECTOR3_H
#define VECTOR3_H

// 3-component vector class and helper functions

#include "util.h"
#include "math_constants.h"

#include <math.h>


class vector3
{
private:
public:
	union
	{
		float v[3];
		struct
		{
			float x, y, z;
		};
	};

	vector3()
	{
	}
	vector3(const float *p) : x(p[0]), y(p[1]), z(p[2])
	{
	}
	vector3(const double *p) : x((float)p[0]), y((float)p[1]), z((float)p[2])
	{
	}
	vector3(const float a, const float b, const float c) : x(a), y(b), z(c)
	{
	}
	vector3(const float a) : x(a), y(a), z(a)
	{
	}
	vector3(const vector3 &a, const vector3 &b)
	{
		v[0] = a.v[0] - b.v[0];
		v[1] = a.v[1] - b.v[1];
		v[2] = a.v[2] - b.v[2];
	}
	// negate
	vector3 operator - () const
	{
		return vector3(-v[0], -v[1], -v[2]);
	}
	void operator += (const vector3 &param)
	{
		v[0] += param.v[0];
		v[1] += param.v[1];
		v[2] += param.v[2];
	}
	void operator += (const float param)
	{
		v[0] += param;
		v[1] += param;
		v[2] += param;
	}
	void operator -= (const vector3 &param)
	{
		v[0] -= param.v[0];
		v[1] -= param.v[1];
		v[2] -= param.v[2];
	}
	void operator -= (const float param)
	{
		v[0] -= param;
		v[1] -= param;
		v[2] -= param;
	}
	void operator *= (const vector3 &param)
	{
		v[0] *= param.v[0];
		v[1] *= param.v[1];
		v[2] *= param.v[2];
	}
	void operator *= (const float param)
	{
		v[0] *= param;
		v[1] *= param;
		v[2] *= param;
	}
	void operator /= (const vector3 &param)
	{
		v[0] /= param.v[0];
		v[1] /= param.v[1];
		v[2] /= param.v[2];
	}
	void operator /= (const float param)
	{
		float ftemp = 1.0f / param;
		v[0] *= ftemp;
		v[1] *= ftemp;
		v[2] *= ftemp;
	}
	vector3& normalize()
	{
		float invlen = 1.0f / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] *= invlen;
		v[1] *= invlen;
		v[2] *= invlen;
		return *this;
	}
	vector3& normalizeFast()
	{
		float invlen = x * x + y * y + z * z;

		float xhalf = 0.5f * invlen;
		int i = *(int*)&invlen;				// get bits for floating value
		// original from quake3 was 0x5f3759df
		// this constant is slightly better
		i = 0x5f375a86 - (i >> 1);		// gives initial guess y0
		invlen = *(float*)&i;				// convert bits back to float
		// can omit this step for about 3.43652% error
		// one step gives about 0.175124% error
		// two steps gives about 4.65437e-004% error
		invlen = invlen * (1.5f - xhalf * invlen * invlen);	// Newton step, repeating increases accuracy

		x *= invlen;
		y *= invlen;
		z *= invlen;

		return *this;
	}
	vector3& zeronormalize()
	{
		float invlen = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		if (invlen == 0)
			return *this;
		invlen = 1.0f / invlen;
		v[0] *= invlen;
		v[1] *= invlen;
		v[2] *= invlen;
		return *this;
	}
	vector3& clamp(float lower, float upper)
	{
		v[0] = v[0] > upper ? upper : v[0];
		v[1] = v[1] > upper ? upper : v[1];
		v[2] = v[2] > upper ? upper : v[2];

		v[0] = v[0] < lower ? lower : v[0];
		v[1] = v[1] < lower ? lower : v[1];
		v[2] = v[2] < lower ? lower : v[2];

		return *this;
	}
	vector3& clamp(const vector3 &clampMin, const vector3 &clampMax)
	{
		x = util::clamp<float>(x, clampMin.x, clampMax.x);
		y = util::clamp<float>(y, clampMin.y, clampMax.y);
		z = util::clamp<float>(z, clampMin.z, clampMax.z);

		return *this;
	}
	float length() const
	{
		return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}
	float length2() const
	{
		return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	}
	bool operator == (vector3 param) const
	{
		if (v[0] == param.v[0] && v[1] == param.v[1] && v[2] == param.v[2])
			return 1;
		return 0;
	}
	bool operator != (vector3 param) const
	{
		if (v[0] != param.v[0] || v[1] != param.v[1] || v[2] != param.v[2])
			return 1;
		return 0;
	}
	vector3& set(const float p1, const float p2, const float p3)
	{
		v[0] = p1; v[1] = p2; v[2] = p3;
		return *this;
	}
};

// *
inline vector3 operator * (const vector3 &a, const vector3 &b)
{
	return vector3(a.v[0] * b.v[0], a.v[1] * b.v[1], a.v[2] * b.v[2]);
}

inline vector3 operator * (const float param, const vector3 &v)
{
	return vector3(v.v[0] * param, v.v[1] * param, v.v[2] * param);
}

inline vector3 operator * (const vector3 &v, const float param)
{
	return vector3(v.v[0] * param, v.v[1] * param, v.v[2] * param);
}

// /
inline vector3 operator / (const vector3 &a, const vector3 &b)
{
	return vector3(a.v[0] / b.v[0], a.v[1] / b.v[1], a.v[2] / b.v[2]);
}

inline vector3 operator / (const float param, const vector3 &v)
{
	return vector3(param / v.v[0], param / v.v[1], param / v.v[2]);
}

inline vector3 operator / (const vector3 &v, const float param)
{
	float invparam = 1.0f / param;
	return vector3(v.v[0] * invparam, v.v[1] * invparam, v.v[2] * invparam);
}

// -
inline vector3 operator - (const vector3 &a, const vector3 &b)
{
	return vector3(a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2]);
}

inline vector3 operator - (const float param, const vector3 &v)
{
	return vector3(param - v.v[0], param - v.v[1], param - v.v[2]);
}

inline vector3 operator - (const vector3 &v, const float param)
{
	return vector3(v.v[0] - param, v.v[1] - param, v.v[2] - param);
}

// +
inline vector3 operator + (const vector3 &a, const vector3 &b)
{
	return vector3(a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2]);
}

inline vector3 operator + (const float param, const vector3 &v)
{
	return vector3(param + v.v[0], param + v.v[1], param + v.v[2]);
}

inline vector3 operator + (const vector3 &v, const float param)
{
	return vector3(v.v[0] + param, v.v[1] + param, v.v[2] + param);
}

inline vector3 VectorSet(const float a, const float b, const float c)
{
	return vector3(a, b, c);
}

inline bool operator < (const vector3 &p1, const vector3 &p2)
{
	if (p1.v[0] < p2.v[0])
		return true;
	else if (p1.v[0] > p2.v[0])
		return false;

	if (p1.v[1] < p2.v[1])
		return true;
	else if (p1.v[1] > p2.v[1])
		return false;

	if (p1.v[2] < p2.v[2])
		return true;
	else if (p1.v[2] > p2.v[2])
		return false;

	return false;
}

inline float Dot(const vector3 &a, const vector3 &b)
{
	return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
}

inline vector3 Cross(const vector3 &a, const vector3 &b)
{
	return vector3(a.v[1] * b.v[2] - a.v[2] * b.v[1],
					a.v[2] * b.v[0] - a.v[0] * b.v[2],
					a.v[0] * b.v[1] - a.v[1] * b.v[0]);
}

inline vector3 Average(const vector3 &a, const vector3 &b)
{
	return vector3((a.v[0] + b.v[0]) * .5f, (a.v[1] + b.v[1]) * .5f, (a.v[2] + b.v[2]) * .5f);
}

inline vector3 NormalVec(const vector3 &a)
{
	float invlen = 1.0f / sqrt(a.v[0] * a.v[0] + a.v[1] * a.v[1] + a.v[2] * a.v[2]);
	return vector3(a.v[0] * invlen, a.v[1] * invlen, a.v[2] * invlen);
}

inline vector3 ZeroNormalVec(const vector3 &a)
{
	float invlen = sqrt(a.v[0] * a.v[0] + a.v[1] * a.v[1] + a.v[2] * a.v[2]);
	if (invlen == 0)
		return a;
	invlen = 1.0f / invlen;
	return vector3(a.v[0] * invlen, a.v[1] * invlen, a.v[2] * invlen);
}

#endif
