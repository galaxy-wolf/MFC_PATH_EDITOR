#include "noise.h"
#include "util.h"
#include "types.h"

#include <math.h>
#include <string.h>


void BicubicFloat(float *in, float *out, int inx, int iny, int outx, int outy)
{
	float tx2, tx3, tx4;
	float ty2, ty3, ty4;
	float x1, x2, x3, x4;
	float y1, y2, y3, y4;
	float xr, yr;
	float w[16];
	int xi, yi;
	int xi1, xi3, xi4;
	int yi1, yi3, yi4;
	float scalex, scaley;
	float invscalex, invscaley;
	float sum1;
	scalex = (float)outx / inx;
	scaley = (float)outy / iny;
	invscalex = 1.0f / scalex;
	invscaley = 1.0f / scaley;
	for (int x = 0; x < outx; x++)
	{
		xi = int(x * invscalex);
		xi1 = xi - 1;
		if (xi1 < 0)
			xi1 += inx;
		xi3 = xi + 1;
		if (xi3 >= inx)
			xi3 -= inx;
		xi4 = xi + 2;
		if (xi4 >= inx)
			xi4 -= inx;

		xr = (float)x * invscalex - xi;


		x1 = .16666666666666666666666666666667f * (1.0f - xr) * (1.0f - xr) * (1.0f - xr);
		tx2 = .16666666666666666666666666666667f * (2.0f - xr) * (2.0f - xr) * (2.0f - xr);
		tx3 = .16666666666666666666666666666667f * (3.0f - xr) * (3.0f - xr) * (3.0f - xr);
		tx4 = .16666666666666666666666666666667f * (4.0f - xr) * (4.0f - xr) * (4.0f - xr);
		x2 = tx2
			- 4.0f * x1;
		x3 = tx3
			- 4.0f * tx2
			+ 6.0f * x1;
		x4 = tx4
			- 4.0f * tx3
			+ 6.0f * tx2
			- 4.0f * x1;

		for (int y = 0; y < outy; y++)
		{
			yi = int(y * invscaley);
			yi1 = yi - 1;
			if (yi1 < 0)
				yi1 += iny;
			yi3 = yi + 1;
			if (yi3 >= iny)
				yi3 -= iny;
			yi4 = yi + 2;
			if (yi4 >= iny)
				yi4 -= iny;

			yr = (float)y * invscaley - yi;


			y1 = .16666666666666666666666666666667f * (1.0f - yr) * (1.0f - yr) * (1.0f - yr);
			ty2 = .16666666666666666666666666666667f * (2.0f - yr) * (2.0f - yr) * (2.0f - yr);
			ty3 = .16666666666666666666666666666667f * (3.0f - yr) * (3.0f - yr) * (3.0f - yr);
			ty4 = .16666666666666666666666666666667f * (4.0f - yr) * (4.0f - yr) * (4.0f - yr);
			y2 = ty2
				- 4.0f * y1;
			y3 = ty3
				- 4.0f * ty2
				+ 6.0f * y1;
			y4 = ty4
				- 4.0f * ty3
				+ 6.0f * ty2
				- 4.0f * y1;

			w[0] = x1 * y1;
			w[1] = x2 * y1;
			w[2] = x3 * y1;
			w[3] = x4 * y1;
			w[4] = x1 * y2;
			w[5] = x2 * y2;
			w[6] = x3 * y2;
			w[7] = x4 * y2;
			w[8] = x1 * y3;
			w[9] = x2 * y3;
			w[10] = x3 * y3;
			w[11] = x4 * y3;
			w[12] = x1 * y4;
			w[13] = x2 * y4;
			w[14] = x3 * y4;
			w[15] = x4 * y4;
			// can reuse the bottom 12 in next for most y
			sum1 = w[0] * in[xi1 * iny + yi1];
			sum1 += w[1] * in[xi * iny + yi1];
			sum1 += w[2] * in[xi3 * iny + yi1];
			sum1 += w[3] * in[xi4 * iny + yi1];
			sum1 += w[4] * in[xi1 * iny + yi];
			sum1 += w[5] * in[xi * iny + yi];
			sum1 += w[6] * in[xi3 * iny + yi];
			sum1 += w[7] * in[xi4 * iny + yi];
			sum1 += w[8] * in[xi1 * iny + yi3];
			sum1 += w[9] * in[xi * iny + yi3];
			sum1 += w[10] * in[xi3 * iny + yi3];
			sum1 += w[11] * in[xi4 * iny + yi3];
			sum1 += w[12] * in[xi1 * iny + yi4];
			sum1 += w[13] * in[xi * iny + yi4];
			sum1 += w[14] * in[xi3 * iny + yi4];
			sum1 += w[15] * in[xi4 * iny + yi4];

			out[x * outy + y] = sum1;
		}
	}
}

void BicubicFloatMAdd(float *in, float *out, int inx, int iny, int outx, int outy, float multiplier)
{
	float tx2, tx3, tx4;
	float ty2, ty3, ty4;
	float x1, x2, x3, x4;
	float y1, y2, y3, y4;
	float xr, yr;
	float w[16];
	int xi, yi;
	int xi1, xi3, xi4;
	int yi1, yi3, yi4;
	float scalex, scaley;
	float invscalex, invscaley;
	float sum1;
	scalex = (float)outx / inx;
	scaley = (float)outy / iny;
	invscalex = 1.0f / scalex;
	invscaley = 1.0f / scaley;
	for (int x = 0; x < outx; x++)
	{
		xi = int(x * invscalex);
		xi1 = xi - 1;
		if (xi1 < 0)
			xi1 += inx;
		xi3 = xi + 1;
		if (xi3 >= inx)
			xi3 -= inx;
		xi4 = xi + 2;
		if (xi4 >= inx)
			xi4 -= inx;

		xr = (float)x * invscalex - xi;


		x1 = .16666666666666666666666666666667f * (1.0f - xr) * (1.0f - xr) * (1.0f - xr);
		tx2 = .16666666666666666666666666666667f * (2.0f - xr) * (2.0f - xr) * (2.0f - xr);
		tx3 = .16666666666666666666666666666667f * (3.0f - xr) * (3.0f - xr) * (3.0f - xr);
		tx4 = .16666666666666666666666666666667f * (4.0f - xr) * (4.0f - xr) * (4.0f - xr);
		x2 = tx2
			- 4.0f * x1;
		x3 = tx3
			- 4.0f * tx2
			+ 6.0f * x1;
		x4 = tx4
			- 4.0f * tx3
			+ 6.0f * tx2
			- 4.0f * x1;

		for (int y = 0; y < outy; y++)
		{
			yi = int(y * invscaley);
			yi1 = yi - 1;
			if (yi1 < 0)
				yi1 += iny;
			yi3 = yi + 1;
			if (yi3 >= iny)
				yi3 -= iny;
			yi4 = yi + 2;
			if (yi4 >= iny)
				yi4 -= iny;

			yr = (float)y * invscaley - yi;


			y1 = .16666666666666666666666666666667f * (1.0f - yr) * (1.0f - yr) * (1.0f - yr);
			ty2 = .16666666666666666666666666666667f * (2.0f - yr) * (2.0f - yr) * (2.0f - yr);
			ty3 = .16666666666666666666666666666667f * (3.0f - yr) * (3.0f - yr) * (3.0f - yr);
			ty4 = .16666666666666666666666666666667f * (4.0f - yr) * (4.0f - yr) * (4.0f - yr);
			y2 = ty2
				- 4.0f * y1;
			y3 = ty3
				- 4.0f * ty2
				+ 6.0f * y1;
			y4 = ty4
				- 4.0f * ty3
				+ 6.0f * ty2
				- 4.0f * y1;

			w[0] = x1 * y1;
			w[1] = x2 * y1;
			w[2] = x3 * y1;
			w[3] = x4 * y1;
			w[4] = x1 * y2;
			w[5] = x2 * y2;
			w[6] = x3 * y2;
			w[7] = x4 * y2;
			w[8] = x1 * y3;
			w[9] = x2 * y3;
			w[10] = x3 * y3;
			w[11] = x4 * y3;
			w[12] = x1 * y4;
			w[13] = x2 * y4;
			w[14] = x3 * y4;
			w[15] = x4 * y4;
			// can reuse the bottom 12 in next for most y
			sum1 = w[0] * in[xi1 * iny + yi1];
			sum1 += w[1] * in[xi * iny + yi1];
			sum1 += w[2] * in[xi3 * iny + yi1];
			sum1 += w[3] * in[xi4 * iny + yi1];
			sum1 += w[4] * in[xi1 * iny + yi];
			sum1 += w[5] * in[xi * iny + yi];
			sum1 += w[6] * in[xi3 * iny + yi];
			sum1 += w[7] * in[xi4 * iny + yi];
			sum1 += w[8] * in[xi1 * iny + yi3];
			sum1 += w[9] * in[xi * iny + yi3];
			sum1 += w[10] * in[xi3 * iny + yi3];
			sum1 += w[11] * in[xi4 * iny + yi3];
			sum1 += w[12] * in[xi1 * iny + yi4];
			sum1 += w[13] * in[xi * iny + yi4];
			sum1 += w[14] * in[xi3 * iny + yi4];
			sum1 += w[15] * in[xi4 * iny + yi4];

			out[x * outy + y] += sum1 * multiplier;
		}
	}
}

void Perturb(float *dest, float *src, float *dh, float *dw, int dimh, int dimw, float magnitude)
{
	float maxdist = magnitude * util::min(dimh, dimw);
	int distbias = int(-maxdist * .5f);

	for (int h = 0; h < dimh; h++)
	{
		for (int w = 0; w < dimw; w++)
		{
			int dh1, dh2, dw1, dw2;
			float dhr, dwr;
			dh1 = int(maxdist * dh[h * dimw + w]);
			dh2 = int(maxdist * dh[h * dimw + w] + 1.0f);
			dhr = maxdist * dh[h * dimw + w] - dh1;
			// bias to a zero center after getting remainder to avoid truncation issue
			dh1 += distbias;
			dh2 += distbias;
			dw1 = int(maxdist * dw[h * dimw + w]);
			dw2 = int(maxdist * dw[h * dimw + w] + 1.0f);
			dwr = maxdist * dw[h * dimw + w] - dw1;
			dw1 += distbias;
			dw2 += distbias;

			dest[h * dimw + w] = (src[(dh1 + h + dimh) % dimh * dimw + (dw1 + w + dimw) % dimw] * (1.0f - dhr)
								+ src[(dh2 + h + dimh) % dimh * dimw + (dw1 + w + dimw) % dimw] * dhr) * (1.0f - dwr)
								+ (src[(dh1 + h + dimh) % dimh * dimw + (dw2 + w + dimw) % dimw] * (1.0f - dhr)
								+ src[(dh2 + h + dimh) % dimh * dimw + (dw2 + w + dimw) % dimw] * dhr) * dwr;
		}
	}
}

#ifdef COMPILER_VS
// gridsize must be power of 2
// p must be 4 * count elements
// p, out, and grid must be 16-byte aligned
// out is added to, so clear to 0 if needed
// out is not normalized
#pragma warning(push)
// frame pointer register 'ebx' modified by inline assembly code
#pragma warning(disable: 4731)
void Perlin3DSSEArray(const float *p, float *out, const int count, const float *grid, const int gridsize, const int levels)
{
	int i, n;
	int _levels = levels;
	int _count = count;
	int gridsize2 = gridsize * gridsize;
	const float *agrid, *ap, *aout;
	ap = p;
	aout = out;
	agrid = grid;
	__declspec(align(16)) float aconstant[20], aftemp[8];
	__declspec(align(16)) int aiconstant[6];
	__declspec(align(16)) float aoutscale = 1.0f;
	__declspec(align(16)) float aoutscalestart = 1.0f;

	aconstant[0] = 1.0f;
	aconstant[1] = 1.0f;
	aconstant[2] = 1.0f;
	aconstant[3] = 1.0f;
	aconstant[4] = 0.5f;
	aconstant[5] = 0.5f;
	aconstant[6] = 0.5f;
	aconstant[7] = 0.5f;
	aconstant[8] = 2.0f;
	aconstant[9] = 2.0f;
	aconstant[10] = 2.0f;
	aconstant[11] = 2.0f;
	aconstant[12] = 3.0f;
	aconstant[13] = 3.0f;
	aconstant[14] = 3.0f;
	aconstant[15] = 3.0f;
	aconstant[16] = (float)gridsize;
	aconstant[17] = (float)gridsize;
	aconstant[18] = (float)gridsize;
	aconstant[19] = (float)gridsize;

	aiconstant[0] = 1;
	aiconstant[1] = 1;
	aiconstant[2] = gridsize - 1;
	aiconstant[3] = gridsize - 1;
	aiconstant[4] = gridsize2;
	aiconstant[5] = gridsize;

	/*float v[3];
	v[0] = (p[0] + 1.0f) * .5f;
	v[1] = (p[1] + 1.0f) * .5f;
	v[2] = (p[2] + 1.0f) * .5f;

	int gridsize2 = gridsize * gridsize;

	int h1, h2, w1, w2, d1, d2;
	float hr, wr, dr;
	float sh, sw, sd;
	for (int i = 0; i < levels; i++)
	{
		h1 = fmod(v[0], 1.0f) * gridsize;
		h2 = (h1 + 1) % gridsize;
		hr = fmod(v[0], 1.0f) * gridsize - h1;
		sh = hr * hr * (3 - 2 * hr);

		w1 = fmod(v[1], 1.0f) * gridsize;
		w2 = (w1 + 1) % gridsize;
		wr = fmod(v[1], 1.0f) * gridsize - w1;
		sw = wr * wr * (3 - 2 * wr);

		d1 = fmod(v[2], 1.0f) * gridsize;
		d2 = (d1 + 1) % gridsize;
		dr = fmod(v[2], 1.0f) * gridsize - d1;
		sd = dr * dr * (3 - 2 * dr);

		out += outscale * (
						((grid[h1 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
						+ (grid[h1 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * (1 - sh)
						+ ((grid[h2 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
						+ (grid[h2 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * sh
						);

		outscale *= .5f;
		v[0] *= 2.0f;
		v[1] *= 2.0f;
		v[2] *= 2.0f;
	}*/


	__asm
	{
		PUSHA
	}

	/*_asm
	{
		RDTSC
		MOV t1, eax
	}*/

	__asm
	{
		MOV n, 0
perlin3dsse_l2:
		MOVAPS xmm7, aconstant		// 1
		MOV eax, ap
		MOV ebx, n
		IMUL ebx, 16
		MOVAPS xmm0, [eax + ebx]
		MOVAPS xmm6, aconstant + 16	// .5
		MOV edi, agrid
		ADDPS xmm0, xmm7
		MOVAPS xmm5, aconstant + 32 // 2
		MULPS xmm0, xmm6			// v = (p + 1) * .5
		MOVQ mm7, aiconstant		// 1
		MOVQ mm6, aiconstant + 8	// gridsize - 1
		MOVQ mm5, aiconstant + 16	// gridsize2, gridsize

		MOV eax, aoutscalestart
		MOV aoutscale, eax

		//XOR eax, eax				// i = 0
		MOV i, 0
perlin3dsse_l1:
		MOVAPS xmm3, aconstant + 48	// 3
		MOVAPS xmm4, aconstant + 64 // gridsize
		MOVAPS xmm7, aconstant		// 1

		// clamp to [0, 1) (assuming original value is [0, 1) multiplied by 2)
		MOVAPS xmm1, xmm0
		CMPPS xmm1, xmm7, 0x5		// not less than 1
		ANDPS xmm1, xmm7
		SUBPS xmm0, xmm1			// subtract 1 if greater than 1
		// scale by gridsize
		MOVAPS xmm1, xmm0
		MULPS xmm1, xmm4

		// truncate to h1 (store in two mmx registers)
		MOVHLPS xmm2, xmm1
		CVTTPS2PI mm0, xmm1
		CVTTPS2PI mm1, xmm2
		// add 1 to get h2
		PSHUFW mm2, mm0, 0xE4
//		PADDD mm2, mm7
//		PSHUFW mm3, mm1, 0xE4
//		PAND mm2, mm6				// mod gridsize
//		PADDD mm3, mm7
//		PAND mm3, mm6				// mod gridsize

		// load truncated (h1)
		CVTPI2PS xmm2, mm1
		PADDD mm2, mm7
		MOVLHPS xmm2, xmm2
		PSHUFW mm3, mm1, 0xE4
		CVTPI2PS xmm2, mm0
		PAND mm2, mm6				// mod gridsize
		// get fraction
		SUBPS xmm1, xmm2
		PADDD mm3, mm7
		// interpolant value (hr * hr * (3 - 2 * hr))
		MOVAPS xmm2, xmm1
		PAND mm3, mm6				// mod gridsize
		MULPS xmm1, xmm1
		PMULLW mm0, mm5				// scale by gridsize2 and gridsize
		MULPS xmm2, xmm5
		PMULLW mm2, mm5				// scale by gridsize2 and gridsize
		SUBPS xmm3, xmm2
		MOVD ebx, mm0
		PSHUFW mm0, mm0, 0x4E		// swap dwords
		MOVAPS xmm2, xmm7			// 1
		MOVD ecx, mm0
		MULPS xmm1, xmm3
		MOVD edx, mm1
		SUBPS xmm2, xmm1			// 1 - interpolant

		/*out += outscale * (
			((grid[h1 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
			+ (grid[h1 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * (1 - sh)
			+ ((grid[h2 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
			+ (grid[h2 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * sh
			);*/

		// get indices
//		PMULLW mm0, mm5				// scale by gridsize2 and gridsize
//		PMULLW mm2, mm5				// scale by gridsize2 and gridsize

//		MOVD ebx, mm0
//		PSHUFW mm0, mm0, 0x4E		// swap dwords
//		MOVD ecx, mm0
//		MOVD edx, mm1

		// h1, w1, d1
		LEA eax, [ebx + ecx]
		ADD eax, edx
		MOV eax, [edi + eax * 4]
		MOV aftemp, eax

		MOVD esi, mm3
		// h1, w1, d2
		LEA eax, [ebx + ecx]
		ADD eax, esi
		MOV eax, [edi + eax * 4]
		MOV aftemp + 16, eax

		PSHUFW mm2, mm2, 0x4E		// swap dwords
		MOVD ecx, mm2
		// h1, w2, d2
		LEA eax, [ebx + ecx]
		ADD eax, esi
		MOV eax, [edi + eax * 4]
		MOV aftemp + 20, eax

		// h1, w2, d1
		LEA eax, [ebx + ecx]
		ADD eax, edx
		MOV eax, [edi + eax * 4]
		MOV aftemp + 4, eax

		PSHUFW mm2, mm2, 0x4E		// swap dwords
		MOVD ebx, mm2
		// h2, w2, d1
		LEA eax, [ebx + ecx]
		ADD eax, edx
		MOV eax, [edi + eax * 4]
		MOV aftemp + 12, eax

		// h2, w2, d2
		LEA eax, [ebx + ecx]
		ADD eax, esi
		MOV eax, [edi + eax * 4]
		MOV aftemp + 28, eax

		MOVD ecx, mm0
		// h2, w1, d2
		LEA eax, [ebx + ecx]
		ADD eax, esi
		MOV eax, [edi + eax * 4]
		MOV aftemp + 24, eax

		// h2, w1, d1
		LEA eax, [ebx + ecx]
		ADD eax, edx
		MOV eax, [edi + eax * 4]
		MOV aftemp + 8, eax

		/*out += outscale * (
			((grid[h1 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
			+ (grid[h1 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * (1 - sh)
			+ ((grid[h2 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
			+ (grid[h2 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * sh
			);*/

		// xmm1 = interpolant, xmm2 = 1 - interpolant
		// put together...
		MOVAPS xmm3, aftemp
		MOVAPS xmm4, aftemp + 16

		// (1-sd) + sd
		SHUFPS xmm5, xmm2, 0xA0			// replicate 1 (from 3rd position)
		MOVHLPS xmm5, xmm5				// replicate 2
		MULPS xmm3, xmm5
		SUBPS xmm7, xmm5
		MULPS xmm4, xmm7
		ADDPS xmm3, xmm4

		// interleave 1 - sw, sw
		SHUFPS xmm4, xmm2, 0x50
		SHUFPS xmm5, xmm1, 0x50
		UNPCKHPS xmm4, xmm5
		MULPS xmm3, xmm4

		// add 1,2 and 3,4
		MOVAPS xmm4, xmm3
		SHUFPS xmm4, xmm4, 0xB1
		ADDPS xmm3, xmm4
		MOVHLPS xmm4, xmm3
		// 1-sh, sh
		MULSS xmm3, xmm2
		MULSS xmm4, xmm1
		// add
		ADDSS xmm3, xmm4
		// scale
		MULSS xmm3, aoutscale
		MOV eax, n
		MOV ebx, aout
		// store
		ADDSS xmm3, [ebx + eax * 4]
		MOVSS [ebx + eax * 4], xmm3


		/*outscale *= .5f;
		v[0] *= 2.0f;
		v[1] *= 2.0f;
		v[2] *= 2.0f;*/
		MOVSS xmm1, aoutscale
		MOVAPS xmm5, aconstant + 32 // 2
		MULSS xmm1, xmm6
		MOVSS aoutscale, xmm1
		MULPS xmm0, xmm5



		MOV eax, i
		ADD eax, 1					// loop i
		CMP eax, _levels
		MOV i, eax
		JNZ perlin3dsse_l1


		MOV eax, n
		ADD eax, 1					// loop n
		CMP eax, _count
		MOV n, eax
		JNZ perlin3dsse_l2
	}


	/*_asm
	{
		RDTSC
		MOV t2, eax
	}
	// timing takes 5 cycles
	if (t2 - t1 < p3d_time)
		p3d_time = t2 - t1;*/

	__asm
	{
		POPA
		EMMS
	}
}
#pragma warning(pop)
#endif

// -1 to 1 input
float Perlin3D(const float *p, const float *grid, const int gridsize, float outscale, const int levels = 1)
{
	float out = 0;

	float v[3];
	v[0] = (p[0] + 1.0f) * .5f;
	v[1] = (p[1] + 1.0f) * .5f;
	v[2] = (p[2] + 1.0f) * .5f;

	int gridsize2 = gridsize * gridsize;

	int h1, h2, w1, w2, d1, d2;
	float hr, wr, dr;
	float sh, sw, sd;
	for (int i = 0; i < levels; i++)
	{
		h1 = int(fmod(v[0], 1.0f) * gridsize);
		h2 = (h1 + 1) % gridsize;
		hr = fmod(v[0], 1.0f) * gridsize - h1;
		sh = hr * hr * (3 - 2 * hr);

		w1 = int(fmod(v[1], 1.0f) * gridsize);
		w2 = (w1 + 1) % gridsize;
		wr = fmod(v[1], 1.0f) * gridsize - w1;
		sw = wr * wr * (3 - 2 * wr);

		d1 = int(fmod(v[2], 1.0f) * gridsize);
		d2 = (d1 + 1) % gridsize;
		dr = fmod(v[2], 1.0f) * gridsize - d1;
		sd = dr * dr * (3 - 2 * dr);


		out += outscale * (
						((grid[h1 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
						+ (grid[h1 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * (1 - sh)
						+ ((grid[h2 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
						+ (grid[h2 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * sh
						);


		outscale *= .5f;
		v[0] *= 2.0f;
		v[1] *= 2.0f;
		v[2] *= 2.0f;
	}

	return out;
}

// 0-1 input
float Perlin3D2(const float *p, const float *grid, const int gridsize, float outscale, const int levels = 1)
{
	float out = 0;

	float v[3];
	v[0] = p[0];
	v[1] = p[1];
	v[2] = p[2];

	int gridsize2 = gridsize * gridsize;

	int h1, h2, w1, w2, d1, d2;
	float hr, wr, dr;
	float sh, sw, sd;
	for (int i = 0; i < levels; i++)
	{
		h1 = int(fmod(v[0], 1.0f) * gridsize);
		h2 = (h1 + 1) % gridsize;
		hr = fmod(v[0], 1.0f) * gridsize - h1;
		sh = hr * hr * (3 - 2 * hr);

		w1 = int(fmod(v[1], 1.0f) * gridsize);
		w2 = (w1 + 1) % gridsize;
		wr = fmod(v[1], 1.0f) * gridsize - w1;
		sw = wr * wr * (3 - 2 * wr);

		d1 = int(fmod(v[2], 1.0f) * gridsize);
		d2 = (d1 + 1) % gridsize;
		dr = fmod(v[2], 1.0f) * gridsize - d1;
		sd = dr * dr * (3 - 2 * dr);


		out += outscale * (
						((grid[h1 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
						+ (grid[h1 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h1 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * (1 - sh)
						+ ((grid[h2 * gridsize2 + w1 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w1 * gridsize + d2] * sd) * (1 - sw)
						+ (grid[h2 * gridsize2 + w2 * gridsize + d1] * (1 - sd) + grid[h2 * gridsize2 + w2 * gridsize + d2] * sd) * sw) * sh
						);


		outscale *= .5f;
		v[0] *= 2.0f;
		v[1] *= 2.0f;
		v[2] *= 2.0f;
	}

	return out;
}

void Perlin3DArray(const float *p, float *out, const int count, const float *grid, const int gridsize, const int levels)
{
#ifdef COMPILER_VS
	Perlin3DSSEArray(p, out,  count, grid, gridsize, levels);
#else
	for (int n = 0; n < count; n++)
	{
		*(out + n) = Perlin3D(p + n * 4, grid, gridsize, 1.0f, levels);
	}
#endif
}
