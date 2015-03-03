#ifndef NOISE_H
#define NOISE_H

#include <string.h>
#include "util.h"
#include "platform.h"


void BicubicFloat(float *in, float *out, int inx, int iny, int outx, int outy);
void BicubicFloatMAdd(float *in, float *out, int inx, int iny, int outx, int outy, float multiplier);

void Perturb(float *dest, float *src, float *dh, float *dw, int dimh, int dimw, float magnitude);

// -1 to 1 input
float Perlin3D(const float *p, const float *grid, const int gridsize, float outscale, const int levels);
// 0 to 1 input
float Perlin3D2(const float *p, const float *grid, const int gridsize, float outscale, const int levels);

void Perlin3DArray(const float *p, float *out, const int count, const float *grid, const int gridsize, const int levels = 1);

#endif
