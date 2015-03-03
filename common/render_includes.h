#ifndef RENDER_INCLUDES_H
#define RENDER_INCLUDES_H

#include "platform.h"


#if defined(OS_WINDOWS)

// need to call glewInit in whatever wrapper launch code I use for win32
# include <GL/glew.h>

# ifndef GL_MAX_SAMPLES_EXT
#  define GL_MAX_SAMPLES_EXT                     0x8D57
# endif

# define glDiscardFramebufferEXT(a, b, c)

#elif defined(OS_MAC)

# if defined (PLATFORM_IPHONE)
#  if defined (RENDER_GLES2)
#   include <OpenGLES/ES2/gl.h>
#   include <OpenGLES/ES2/glext.h>

// texture formats
#   define GL_LUMINANCE8 GL_LUMINANCE
#   define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA
#   define GL_RGB8 GL_RGB8_OES
#   define GL_RGBA8 GL_RGBA8_OES

// mapbuffer
#   define glMapBuffer glMapBufferOES
#   define glUnmapBuffer glUnmapBufferOES
#   define GL_WRITE_ONLY GL_WRITE_ONLY_OES

// framebuffer_object
#   define glGenFramebuffersEXT glGenFramebuffers
#   define glDeleteFramebuffersEXT glDeleteFramebuffers
#   define glGenRenderbuffersEXT glGenRenderbuffers
#   define glDeleteRenderbuffersEXT glDeleteRenderbuffers
#   define glBindFramebufferEXT glBindFramebuffer
#   define glBindRenderbufferEXT glBindRenderbuffer
#   define glFramebufferRenderbufferEXT glFramebufferRenderbuffer
#   define glFramebufferTexture2DEXT glFramebufferTexture2D
#   define glRenderbufferStorageEXT glRenderbufferStorage
#   define glGetRenderbufferParameterivEXT glGetRenderbufferParameteriv
#   define glCheckFramebufferStatusEXT glCheckFramebufferStatus
#   define glGenerateMipmapEXT glGenerateMipmap
#   define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER
#   define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
#   define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0
#   define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT
#   define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
#   define GL_RENDERBUFFER_WIDTH_EXT GL_RENDERBUFFER_WIDTH
#   define GL_RENDERBUFFER_HEIGHT_EXT GL_RENDERBUFFER_HEIGHT
#   define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE

// multisampling
#   define glRenderbufferStorageMultisampleEXT glRenderbufferStorageMultisampleAPPLE
#   define GL_MAX_SAMPLES_EXT GL_MAX_SAMPLES_APPLE
#   define GL_READ_FRAMEBUFFER_EXT GL_READ_FRAMEBUFFER_APPLE
#   define GL_DRAW_FRAMEBUFFER_EXT GL_DRAW_FRAMEBUFFER_APPLE

#  else // GLES1.1
#   include <OpenGLES/ES1/gl.h>
#   include <OpenGLES/ES1/glext.h>

// multitexture
#   define GL_TEXTURE0_ARB GL_TEXTURE0
#   define GL_TEXTURE1_ARB GL_TEXTURE1
#   define glActiveTextureARB glActiveTexture

// texture formats
#   define GL_LUMINANCE8 GL_LUMINANCE
#   define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA
#   define GL_RGB8 GL_RGB8_OES
#   define GL_RGBA8 GL_RGBA8_OES

#   define GL_GENERATE_MIPMAP_SGIS GL_GENERATE_MIPMAP

// framebuffer_object
#   define glGenFramebuffersEXT glGenFramebuffersOES
#   define glDeleteFramebuffersEXT glDeleteFramebuffersOES
#   define glGenRenderbuffersEXT glGenRenderbuffersOES
#   define glDeleteRenderbuffersEXT glDeleteRenderbuffersOES
#   define glBindFramebufferEXT glBindFramebufferOES
#   define glBindRenderbufferEXT glBindRenderbufferOES
#   define glFramebufferRenderbufferEXT glFramebufferRenderbufferOES
#   define glRenderbufferStorageEXT glRenderbufferStorageOES
#   define glGetRenderbufferParameterivEXT glGetRenderbufferParameterivOES
#   define glCheckFramebufferStatusEXT glCheckFramebufferStatusOES
#   define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER_OES
#   define GL_RENDERBUFFER_EXT GL_RENDERBUFFER_OES
#   define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0_OES
#   define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT_OES
#   define GL_DEPTH_COMPONENT16 GL_DEPTH_COMPONENT16_OES
#   define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
#   define GL_RENDERBUFFER_WIDTH_EXT GL_RENDERBUFFER_WIDTH_OES
#   define GL_RENDERBUFFER_HEIGHT_EXT GL_RENDERBUFFER_HEIGHT_OES
#   define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE_OES
#   define GL_FRAMEBUFFER_BINDING_EXT GL_FRAMEBUFFER_BINDING_OES

// point sprites / point size
#   define GL_POINT_SPRITE_ARB GL_POINT_SPRITE_OES
#   define GL_COORD_REPLACE_ARB GL_COORD_REPLACE_OES
#  endif

# else // regular mac
#  include <OpenGL/gl.h>
#  include <OpenGL/glext.h>
#  define glBindFragDataLocation glBindFragDataLocationEXT
#  define GL_MIN_PROGRAM_TEXEL_OFFSET GL_MIN_PROGRAM_TEXEL_OFFSET_EXT
#  define GL_MAX_PROGRAM_TEXEL_OFFSET GL_MAX_PROGRAM_TEXEL_OFFSET_EXT
# endif

#else

# error "platform not implemented"

#endif

#endif
