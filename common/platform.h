#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
# define OS_WINDOWS

// compilers for this platform
# ifdef _MSC_VER
#  define COMPILER_VS
#  define COMPILER_VC
# endif
#endif

#if defined(__APPLE_CPP__) || defined(__APPLE__) || defined(__APPLE_CC__) || defined(__MACH__) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
# define OS_MAC

# include <TargetConditionals.h>

# if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
#  define PLATFORM_IPHONE_SIM
#  define PLATFORM_IPHONE
# elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#  define PLATFORM_IPHONE
# else
#  define PLATFORM_OSX
# endif

// compilers for this platform
# if defined(__GNUC__)
#  define COMPILER_GCC
# endif
#endif

#endif
