#ifndef VIS_TESTS_H
#define VIS_TESTS_H

#include "vector3.h"

namespace VisTest
{

enum Result
{
	ResultOut = 0,
	ResultPartial,
	ResultIn
};

struct Plane
{
	vector3 n;
	float d;
};

// returns 2 for completely inside
// 1 for partially inside
// 0 for completely outside
template<int planes>
Result FrustumSphere(const Plane* const plane, const vector3 &point, const float radius)
{
	bool partial = false;
	for (int n = 0; n < planes; n++)
	{
		float d2 = plane[n].d + Dot(plane[n].n, point);
		if (d2 < -radius)
			return ResultOut;
		if (d2 < radius)
			partial = true;
	}
	if (partial)
		return ResultPartial;
	return ResultIn;
}

// returns 2 for completely inside
// 1 for partially inside
// 0 for completely outside
template<int planes>
Result FrustumAABB(const Plane* const plane, const vector3 &min, const vector3 &max)
{
	float d2;
	int count, partial = 0;
	for (int n = 0; n < planes; n++)
	{
		count = 0;

		d2 = plane[n].d + Dot(plane[n].n, vector3(min.x, min.y, min.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(max.x, min.y, min.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(min.x, max.y, min.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(min.x, min.y, max.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(max.x, max.y, min.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(max.x, min.y, max.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(min.x, max.y, max.z));
		if (d2 < 0)
			count++;
		d2 = plane[n].d + Dot(plane[n].n, vector3(max.x, max.y, max.z));
		if (d2 < 0)
			count++;

		if (count == 8) // completely out
			return ResultOut;
		else if (count > 0) // partially out
			partial = 1;
	}

	if (partial)
		return ResultPartial;
	return ResultIn;
}

} // namespace VisTest

#endif
