#ifndef NOISE_VORONOI_H
#define NOISE_VORONOI_H

#include "util.h"

#include <float.h>
#include <algorithm>

// other combinations of d1*c1 + d2*c2 + d3*c3...dn*cn
// can be made faster with spatial partitioning
template <int outdimX, int outdimY, int cellCount>
void Voronoi2D(float *height, const int *cellPos, float bias)
{
	for (int h = 0; h < outdimY; h++)
	{
		for (int w = 0; w < outdimX; w++)
		{
			int d0 = outdimX * outdimY;
			int d1 = outdimX * outdimY;
			for (int n = 0; n < cellCount; n++)
			{
				int dh, dw;
				dh = abs(h - cellPos[n * 2 + 0]);
				if (dh > outdimY / 2)
					dh = outdimY - dh;
				dw = abs(w - cellPos[n * 2 + 1]);
				if (dw > outdimX / 2)
					dw = outdimX - dw;

				int d = dh * dh + dw * dw;
				if (d < d0)
				{
					d1 = d0;
					d0 = d;
				}
				else if (d < d1)
				{
					d1 = d;
				}
			}
			height[h * outdimX + w] = float(util::max(0, -d0 + d1));
		}
	}

	float low = FLT_MAX;
	float high = -FLT_MAX;
	for (int n = 0; n < outdimX * outdimY; n++)
	{
		low = util::min(low, height[n]);
		high = util::max(high, height[n]);
	}
	float scale = 1.0f / (high - low);
	for (int n = 0; n < outdimX * outdimY; n++)
	{
		height[n] = util::clamp((height[n] - low) * scale + bias, 0.0f, 1.0f);
	}
}

#endif
