#ifndef MATH_H
#define MATH_H

#include "util.h"
#include "vector3.h"

#include <float.h>


namespace util
{

inline float IntersectRayPlane(const vector3 &origin, const vector3 &dir, const vector3 &planePoint, const vector3 &planeNormal)
{
	//origin + dir * t
	float t = (Dot(planePoint, planeNormal) - Dot(origin, planeNormal)) / Dot(planeNormal, dir);
	return t;
}

inline int IntersectRaySphere(const vector3 &ray_origin, const vector3 &ray_dir, const vector3 &sphere_center, const float sphere_radius, float &t, vector3 &int_point)
{
	vector3 m = ray_origin - sphere_center;
	float b = Dot(m, ray_dir);
	float c = m.length2() - sphere_radius * sphere_radius;
	// exit if ray origin outside sphere and ray pointing away from sphere
	if (c > 0.0f && b > 0.0f)
		return 0;
	float discr = b * b - c;
	// negative discriminant means no intersection
	if (discr < 0.0f)
		return 0;
	// we have an intersection, calculate the nearest of the two
	t = -b - sqrt(discr);
	// if t is negative the ray origin is inside the sphere, clamp to zero
	if (t < 0.0f)
		t = 0.0f;
	int_point = ray_origin + t * ray_dir;
	return 1;
}

inline float IntersectRaySphere2(const vector3 &rO, const vector3 &rV, const vector3 &sO, const float sR)
{
	vector3 Q = sO - rO;

	float c2 = Q.length2();
	float v = Dot(Q, rV);
	float d = sR * sR - (c2 - v * v);

	if (d < 0.0)
		return -1.0f;
	return v - sqrt(d);
}

inline bool IntersectRaySphere3(const vector3 &rO, const vector3 &rV, const vector3 &sO, const float sR2)
{
	vector3 Q = sO - rO;

	float c2 = Q.length2();
	float v = Dot(Q, rV);
	float d = sR2 - (c2 - v * v);

	if (d < 0)
		return false;
	return true;
}

inline bool IntersectRayAABB(const vector3 &rayOrigin, const vector3 &rayDir, const vector3 &aabbMin, const vector3 &aabbMax, float &tMin, float &tMax)
{
	tMin = 0;
	tMax = FLT_MAX;
	const float epsilon = 0.000001f;

	for (int i = 0; i < 3; i++)
	{
		if (fabs(rayDir.v[i]) < epsilon)
		{
			// ray is parallel to slab, no hit if origin not within slab
			if (rayOrigin.v[i] < aabbMin.v[i] || rayOrigin.v[i] > aabbMax.v[i])
				return false;
		}
		else
		{
			// compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / rayDir.v[i];
			float t1 = (aabbMin.v[i] - rayOrigin.v[i]) * ood;
			float t2 = (aabbMax.v[i] - rayOrigin.v[i]) * ood;
			// make t1 and t2 be the near and far intersection points
			if (t1 > t2)
				swap(t1, t2);
			// compute the intersection of slab intersection intervals
			if (t1 > tMin)
				tMin = t1;
			if (t2 < tMax)
				tMax = t2;
			// exit with no collision as soon as slab intersection becomes empty
			if (tMin > tMax)
				return false;
		}
	}

	// ray intersects all three slabs
	return true;
}

inline bool IntersectRayTriangle(
	const vector3 &rayOrigin, const vector3 &rayDir,
	const vector3 &a, const vector3 &b, const vector3 &c,
	float &u, float &v, float &w, float &t)
{
	vector3 ab = b - a;
	vector3 ac = c - a;
	vector3 qp = -rayDir;

	vector3 n = Cross(ab, ac);

	float d = Dot(qp, n);
	if (d <= 0)
		return false;

	vector3 ap = rayOrigin - a;
	t = Dot(ap, n);
	if (t < 0)
		return false;

	vector3 e = Cross(qp, ap);
	v = Dot(ac, e);
	if (v < 0 || v > d)
		return false;
	w = -Dot(ab, e);
	if (w < 0 || v + w > d)
		return false;

	float invMag = 1.0f / d;
	t *= invMag;
	v *= invMag;
	w *= invMag;
	u = 1.0f - v - w;

	return true;
}

inline vector3 ClosestPointAABB(const vector3 &aabbMin, const vector3 &aabbMax, const vector3 &point)
{
	vector3 closest = point;

	// clamp the point's invidual axes to the box planes
	closest.x = clamp(closest.x, aabbMin.x, aabbMax.x);
	closest.y = clamp(closest.y, aabbMin.y, aabbMax.y);
	closest.z = clamp(closest.z, aabbMin.z, aabbMax.z);

	return closest;
}

// squared distance
inline float DistanceSqPointAABB(const vector3 &aabbMin, const vector3 &aabbMax, const vector3 &point)
{
	float d2 = 0.0f;

	// find the squared distance between the point and AABB
	for (int n = 0; n < 3; n++)
	{
		if (point.v[n] < aabbMin.v[n])
		{
			float s = point.v[n] - aabbMin.v[n];
			d2 += s * s;
		}
		else if (point.v[n] > aabbMax.v[n])
		{
			float s = point.v[n] - aabbMax.v[n];
			d2 += s * s;
		}
	}

	return d2;
}

// squared distance
inline float DistanceSqPointAABB2D(float aabbMinX, float aabbMinY, float aabbMaxX, float aabbMaxY, float pointX, float pointY)
{
	float d2 = 0.0f;

	// find the squared distance between the point and AABB
	if (pointX < aabbMinX)
	{
		float s = pointX - aabbMinX;
		d2 += s * s;
	}
	else if (pointX > aabbMaxX)
	{
		float s = pointX - aabbMaxX;
		d2 += s * s;
	}
	if (pointY < aabbMinY)
	{
		float s = pointY - aabbMinY;
		d2 += s * s;
	}
	else if (pointY > aabbMaxY)
	{
		float s = pointY - aabbMaxY;
		d2 += s * s;
	}

	return d2;
}

inline bool IntersectSphereAABB(const vector3 &aabbMin, const vector3 &aabbMax, const vector3 &sphereOrigin, float sphereRadius)
{
	float d = DistanceSqPointAABB(aabbMin, aabbMax, sphereOrigin);

	return d <= sphereRadius * sphereRadius;
}

inline bool IntersectLineSegments2D(
	float l0p0x, float l0p0y, float l0p1x, float l0p1y,
	float l1p0x, float l1p0y, float l1p1x, float l1p1y,
	float &ix, float &iy)
{
	float ux = l0p1x - l0p0x;
	float uy = l0p1y - l0p0y;
	float vx = l1p1x - l1p0x;
	float vy = l1p1y - l1p0y;

	float d = ux * vy - uy * vx;

	if (d < 0)
		d = -d;

	if (d < .000001f)
		return false; // parallel

	float wx = l0p0x - l1p0x;
	float wy = l0p0y - l1p0y;

	float s = vx * wy - vy * wx;
	if (s < 0 || s > d)
		return false;

	float t = ux * wy - uy * wx;
	if (t < 0 || t > d)
		return false;

	float i = s / d;
	ix = l0p0x + ux * i;
	iy = l0p0y + uy * i;
	return true;
}

// projects origin along dir to AABB boundary if vector is outside of AABB
// returns false if the point is outside the AABB and cannot be projected along dir to it
inline bool ClampToAABB(vector3 &origin, const vector3 &dir, const vector3 &aabbMin, const vector3 &aabbMax)
{
	if (origin.x < aabbMin.x)
	{
		if (dir.x <= 0.0f)
			return false;

		float t = (aabbMin.x - origin.x) / dir.x;
		origin += dir * t;
	}
	else if (origin.x > aabbMax.x)
	{
		if (dir.x >= 0.0f)
			return false;

		float t = (aabbMax.x - origin.x) / dir.x;
		origin += dir * t;
	}

	if (origin.y < aabbMin.y)
	{
		if (dir.y <= 0.0f)
			return false;

		float t = (aabbMin.y - origin.y) / dir.y;
		origin += dir * t;
	}
	else if (origin.y > aabbMax.y)
	{
		if (dir.y >= 0.0f)
			return false;

		float t = (aabbMax.y - origin.y) / dir.y;
		origin += dir * t;
	}

	if (origin.z < aabbMin.z)
	{
		if (dir.z <= 0.0f)
			return false;

		float t = (aabbMin.z - origin.z) / dir.z;
		origin += dir * t;
	}
	else if (origin.z > aabbMax.z)
	{
		if (dir.z >= 0.0f)
			return false;

		float t = (aabbMax.z - origin.z) / dir.z;
		origin += dir * t;
	}

	return true;
}

} // namespace util

#endif