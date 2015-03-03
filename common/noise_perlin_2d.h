#ifndef NOISE_PERLIN_2D_H
#define NOISE_PERLIN_2D_H

#include "util.h"

#include <string.h>

// to get rid of the repeating patterns, use a different/larger grid on subsequent levels
// instead of just tiling it
template <int outdimX, int outdimY, int gridsizeX, int gridsizeY>
void Perlin2D(float *out, const float *grid, int levels)
{
	memset(out, 0, sizeof(float) * outdimX * outdimY);

	// clamp useless noise levels
	int maxLevelsX = util::GetBitIndex(outdimX);
	int maxLevelsY = util::GetBitIndex(outdimY);
	int maxLevels;
	if (maxLevelsX > maxLevelsY)
		maxLevels = maxLevelsX;
	else
		maxLevels = maxLevelsY;
	if (levels > maxLevels || levels == 0)
		levels = maxLevels;


	float outscale = 1;

	float scale = (float)util::max(outdimX, outdimY) / util::max(gridsizeX, gridsizeY);
	int h1, h2, w1, w2;
	float hr, wr;
	float sh, sw;
	for (int i = 0; i < levels; i++)
	{
		if (scale <= 1)
		{
			for (int h = 0; h < outdimY; h++)
			{
				h1 = h % gridsizeY;
				for (int w = 0; w < outdimX; w++)
				{
					w1 = w % gridsizeX;
					out[h * outdimX + w] += grid[h1 * gridsizeX + w1] * outscale;
				}
			}
		}
		else
		{
			for (int h = 0; h < outdimY; h++)
			{
				h1 = (int)(h / scale) % gridsizeY;
				h2 = (h1 + 1) % gridsizeY;
				hr = (float)h / scale - (int)(h / scale);
				sh = hr * hr * (3 - 2 * hr);

				for (int w = 0; w < outdimX; w++)
				{
					w1 = (int)(w / scale) % gridsizeX;
					w2 = (w1 + 1) % gridsizeX;
					wr = (float)w / scale - (int)(w / scale);
					sw = wr * wr * (3 - 2 * wr);

					out[h * outdimX + w] += outscale * ((grid[h1 * gridsizeX + w1] * (1 - sw) + grid[h1 * gridsizeX + w2] * sw) * (1 - sh)
						+ (grid[h2 * gridsizeX + w1] * (1 - sw) + grid[h2 * gridsizeX + w2] * sw) * sh);
				}
			}
		}
		outscale *= .5f;
		scale /= 2;
	}

	float omin = 100000000, omax = 0;
	for (int n = 0; n < outdimX * outdimY; n++)
	{
		if (out[n] > omax)
			omax = out[n];
		if (out[n] < omin)
			omin = out[n];
	}
	float oscale = 1.0f / (omax - omin);
	for (int n = 0; n < outdimX * outdimY; n++)
	{
		out[n] = (out[n] - omin) * oscale;
	}
}

#endif
