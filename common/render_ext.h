#ifndef RENDER_EXT_H
#define RENDER_EXT_H

#include <GL/glew.h>

#ifndef APIENTRY
#  ifdef _WIN32
#    define APIENTRY __stdcall
#  else
#    define APIENTRY
#  endif
#endif

// GL_NVX_conditional_render
typedef void (APIENTRY * PFNGLBEGINCONDITIONALRENDERNVXPROC) (GLuint id);
typedef void (APIENTRY * PFNGLENDCONDITIONALRENDERNVXPROC) (void);
extern PFNGLBEGINCONDITIONALRENDERNVXPROC glBeginConditionalRenderNVX;
extern PFNGLENDCONDITIONALRENDERNVXPROC glEndConditionalRenderNVX;
bool LoadNVXConditionalRender();

// GL_NVX_gpu_memory_info
#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

// GL_ATI_meminfo
#define GL_VBO_FREE_MEMORY_ATI                     0x87FB
#define GL_TEXTURE_FREE_MEMORY_ATI                 0x87FC
#define GL_RENDERBUFFER_FREE_MEMORY_ATI            0x87FD

#endif
