#ifndef UTIL_SSE_H
#define UTIL_SSE_H

#include <emmintrin.h> // SSE2

// reciprocal with refinement
inline __m128 _mm_rcpnr_ps(__m128 x)
{
	__m128 t = _mm_rcp_ps(x);
	return _mm_sub_ps(_mm_add_ps(t, t), _mm_mul_ps(_mm_mul_ps(t, t), x));
}

#endif
