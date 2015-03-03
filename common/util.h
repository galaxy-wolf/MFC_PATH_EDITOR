#ifndef UTIL_H
#define UTIL_H

#include "platform.h"

// offsetof
#if defined(COMPILER_GCC)
# if !defined(offsetof)
#  define offsetof(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)
# endif
#else
# include <stddef.h>
#endif

// prefetch
#ifdef COMPILER_GCC
# define prefetch(x) __builtin_prefetch(x)
#else
# include <xmmintrin.h>
# define prefetch(x) _mm_prefetch((char*)x, _MM_HINT_NTA)
#endif

// endian conversion
#define	REVERSE4(type)	(((type >> 24) & 0x000000ff) | \
						((type >> 8) & 0x0000ff00) | \
						((type << 8) & 0x00ff0000) | \
						((type << 24) & 0xff000000) )
#define	REVERSE2(type)	(((type >> 8) & 0x00ff) | \
						((type << 8) & 0xff00) )

namespace util
{

#undef min
#undef max
template<typename T> inline T min(T a, T b)
{
	return a < b ? a : b;
}
template<typename T> inline T max(T a, T b)
{
	return a > b ? a : b;
}

template<typename T> inline T clamp(T p, T low, T high)
{
	return min(high, max(low, p));
}

template<typename T> inline void swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}

template<typename T> inline T sign(T &p)
{
	if (p > T(0))
		return T(1);
	if (p < T(0))
		return T(-1);
	return T(0);
}

// returns the log base 2, or the position of the highest bit set
int GetBitIndex(unsigned int v);
// PackBits
void RLEUnpack(const char *in, char *out, int inlen);

} // namespace util

#endif
