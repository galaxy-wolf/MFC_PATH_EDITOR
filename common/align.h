#ifndef ALIGN_H
#define ALIGN_H

#include "platform.h"


// alignment macros
#if defined(COMPILER_VC)
# define ALIGN_START(x) __declspec(align(x))
# define ALIGN_END(x)
#elif defined(COMPILER_GCC)
# define ALIGN_START(x)
# define ALIGN_END(x) __attribute__ ((aligned (x)))
#else
# define ALIGN_START(x)
# define ALIGN_END(x)
#endif

#endif
