#include <GL/glew.h>
#include "render_ext.h"

// from glew.c
#if defined(_WIN32)
#  undef APIENTRY
#  include <windows.h>
#  define glewGetProcAddress(name) wglGetProcAddress(name)
#  define GET_PROC_STRING_TYPE LPCSTR
#else
#  if defined(__APPLE__)
#    define glewGetProcAddress(name) NSGLGetProcAddress(name)
#    define GET_PROC_STRING_TYPE const GLubyte*
#  else
#    if defined(__sgi) || defined(__sun)
#      define glewGetProcAddress(name) dlGetProcAddress(name)
#      define GET_PROC_STRING_TYPE const GLubyte*
#    else /* __linux */
#      include <GL/glx.h>
#      define glewGetProcAddress(name) (*glXGetProcAddress)(name)
#      define GET_PROC_STRING_TYPE const GLubyte*
#    endif
#  endif
#endif

PFNGLBEGINCONDITIONALRENDERNVXPROC glBeginConditionalRenderNVX = 0;
PFNGLENDCONDITIONALRENDERNVXPROC glEndConditionalRenderNVX = 0;
bool LoadNVXConditionalRender()
{
	if (!(glBeginConditionalRenderNVX = (PFNGLBEGINCONDITIONALRENDERNVXPROC)glewGetProcAddress((GET_PROC_STRING_TYPE)"glBeginConditionalRenderNVX")))
		return false;
	if (!(glEndConditionalRenderNVX = (PFNGLENDCONDITIONALRENDERNVXPROC)glewGetProcAddress((GET_PROC_STRING_TYPE)"glEndConditionalRenderNVX")))
		return false;
	return true;
}
