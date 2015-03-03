#ifndef NOISE_PERLIN_3D_H
#define NOISE_PERLIN_3D_H


// 0 to 1 input
template <int gridDim> // power of 2 for best performance
float Perlin3D(const float *p, const float *grid, float scale = 1.0f, const int levels = 1)
{
	float out = 0;

	float v[3];
	v[0] = p[0];
	v[1] = p[1];
	v[2] = p[2];

	enum {gridDim2 = gridDim * gridDim};

	for (int i = 0; i < levels; i++)
	{
		int hi, wi, di;
		int h1, h2, w1, w2, d1, d2;
		float hr, wr, dr;
		float sh, sw, sd;

		hi = int(v[0] * gridDim);
		h1 = hi % gridDim;
		h2 = (hi + 1) % gridDim;
		hr = v[0] * gridDim - hi;
		sh = hr * hr * (3.0f - 2.0f * hr);

		wi = int(v[1] * gridDim);
		w1 = wi % gridDim;
		w2 = (wi + 1) % gridDim;
		wr = v[1] * gridDim - wi;
		sw = wr * wr * (3.0f - 2.0f * wr);

		di = int(v[2] * gridDim);
		d1 = di % gridDim;
		d2 = (di + 1) % gridDim;
		dr = v[2] * gridDim - di;
		sd = dr * dr * (3.0f - 2.0f * dr);


		float value =
				  ((grid[h1 * gridDim2 + w1 * gridDim + d1] * (1.0f - sd) + grid[h1 * gridDim2 + w1 * gridDim + d2] * sd) * (1.0f - sw)
				+  (grid[h1 * gridDim2 + w2 * gridDim + d1] * (1.0f - sd) + grid[h1 * gridDim2 + w2 * gridDim + d2] * sd) * sw) * (1.0f - sh)
				+ ((grid[h2 * gridDim2 + w1 * gridDim + d1] * (1.0f - sd) + grid[h2 * gridDim2 + w1 * gridDim + d2] * sd) * (1.0f - sw)
				+  (grid[h2 * gridDim2 + w2 * gridDim + d1] * (1.0f - sd) + grid[h2 * gridDim2 + w2 * gridDim + d2] * sd) * sw) * sh;
		out += value * scale;


		scale *= .5f;
		v[0] *= 2.0f;
		v[1] *= 2.0f;
		v[2] *= 2.0f;
	}

	return out;
}

#endif
