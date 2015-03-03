#ifndef SHADER_GLSL_H
#define SHADER_GLSL_H

#include "platform.h"

#if defined(PLATFORM_IPHONE)
# define RENDER_GLES2
#endif
#include "render_includes.h" // for GS default settings on loadProgram()

#if defined(GL_GEOMETRY_SHADER_ARB)
# define SHADER_ENABLE_GS
#endif
#if defined(GL_TESS_CONTROL_SHADER)
# define SHADER_ENABLE_TS
#endif
#if defined(GL_COMPUTE_SHADER)
# define SHADER_ENABLE_CS
#endif
#if defined(GL_UNIFORM_BUFFER)
# define SHADER_ENABLE_UB
#endif

#if defined(OS_WINDOWS)
# define SHADER_RELOAD
#endif

#ifdef SHADER_RELOAD
# include "critsect.h"
#endif

#include "fixed_string.h"
#include <set>
#include <map>


class UniformBlock;

class GLSLProgram
{
public:
	unsigned int handle;

	enum {maxName = 128};
	char vertexShader[maxName];
#ifdef SHADER_ENABLE_TS
	char tessControlShader[maxName];
	char tessEvalShader[maxName];
#endif
#ifdef SHADER_ENABLE_GS
	char geometryShader[maxName];
#endif
	char fragmentShader[maxName];
#ifdef SHADER_ENABLE_CS
	char computeShader[maxName];
#endif

	enum {maxUniformName = 64};
	mutable std::map<FixedString<maxUniformName>, int> uniformLocations;

	enum {maxLocalSourceAddition = 256};
	char localSourceAddition[maxLocalSourceAddition];

#ifdef SHADER_ENABLE_UB
	enum {maxUniformBlocks = 16};
	UniformBlock *uniformBlock[maxUniformBlocks];
	int uniformBlocks;
#endif

	GLSLProgram()
	{
		handle = 0;

		vertexShader[0] = 0;
#ifdef SHADER_ENABLE_TS
		tessControlShader[0] = 0;
		tessEvalShader[0] = 0;
#endif
#ifdef SHADER_ENABLE_GS
		geometryShader[0] = 0;
#endif
		fragmentShader[0] = 0;
#ifdef SHADER_ENABLE_CS
		computeShader[0] = 0;
#endif

		localSourceAddition[0] = 0;

#ifdef SHADER_ENABLE_UB
		uniformBlocks = 0;
#endif
	}

	void onRelink()
	{
		uniformLocations.clear();
	}

	int getUniformLocation(const char *name) const;
};

class GLSLShaderManager
{
public:
	GLSLShaderManager(const char *_shaderPath = "official_mods/base/shaders/glsl");
	~GLSLShaderManager();

	bool startup();
	void shutdown();

	bool update();

	// gs_in: GL_POINTS, GL_LINES, GL_LINES_ADJACENCY_EXT, GL_TRIANGLES, GL_TRIANGLES_ADJACENCY_EXT
	// gs_out: GL_POINTS, GL_LINE_STRIP, GL_TRIANGLE_STRIP
	// gs_outmax: 1024 vertices on g80
	struct LoadOptions
	{
		const char *vertexShader;
		const char *tessControlShader;
		const char *tessEvalShader;
		const char *geometryShader;
		const char *fragmentShader;
		const char *computeShader;
		const char *localSourceAddition;
		int uniformBlockCount;
		UniformBlock **uniformBlocks;
		GLenum gs_in;
		GLenum gs_out;
		GLenum gs_outmax;

		LoadOptions()
			: vertexShader(nullptr)
			, tessControlShader(nullptr)
			, tessEvalShader(nullptr)
			, geometryShader(nullptr)
			, fragmentShader(nullptr)
			, computeShader(nullptr)
			, localSourceAddition(nullptr)
			, uniformBlockCount(0)
			, uniformBlocks(nullptr)
			, gs_in(GL_TRIANGLES)
			, gs_out(GL_TRIANGLE_STRIP)
			, gs_outmax(3)
		{
		}
	};

	const GLSLProgram* loadProgram(
		const char *vertexShader
		, const char *tessControlShader
		, const char *tessEvalShader
		, const char *geometryShader
		, const char *fragmentShader
		, const char *localSourceAddition = 0
		, int uniformBlockCount = 0
		, UniformBlock **uniformBlocks = 0
		, GLenum gs_in = GL_TRIANGLES
		, GLenum gs_out = GL_TRIANGLE_STRIP
		, GLenum gs_outmax = 3
		);
	const GLSLProgram* loadProgramCompute(
		const char *computeShader
		, const char *localSourceAddition = 0
		, int uniformBlockCount = 0
		, UniformBlock **uniformBlocks = 0
		);
	const GLSLProgram* loadProgram(LoadOptions options);

	int updateFileLoop();

	enum {maxPrograms = 256};
	GLSLProgram program[maxPrograms];
	unsigned int programs;

	void addDefine(const char *name);
	void removeDefine(const char *name);

private:

	int _maxVertexAttribs;
	int _maxTextureImageUnits;
	int _maxDrawBuffers;
	int _shadingLanguageVersion;
	bool _uniformBuffersSupported;

	enum {maxShaderLen = 65536};
	char shaderBuffer[maxShaderLen];
	enum {maxInfoLogLen = 65536};
	char infoLogBuffer[maxInfoLogLen];

	enum {maxDefineLen = 128};
	std::set<FixedString<maxDefineLen> > shaderDefines;

	FixedString<512> shaderPath;

	void *logFile;

	// program fragments
	struct Shader
	{
		unsigned int id;

		enum
		{
			type_none = 0,
			type_vertex,
			type_tess_control,
			type_tess_eval,
			type_geometry,
			type_fragment,
			type_compute
		};
		int type;

		std::set<int> dependent_programs;

		enum {maxName = 128};
		char name[maxName];

		enum {maxLocalSourceAddition = GLSLProgram::maxLocalSourceAddition};
		char localSourceAddition[maxLocalSourceAddition];

#ifdef SHADER_ENABLE_UB		
		enum {maxUniformBlocks = GLSLProgram::maxUniformBlocks};
		UniformBlock *uniformBlock[maxUniformBlocks];
		int uniformBlocks;
#endif
	};
	enum {maxShaders = maxPrograms * 8};
	Shader shader[maxShaders];
	unsigned int shaders;

	void writeLogV(const char *format, va_list argptr);
	void writeLog(const char *format, ...);

	bool loadShader(
		const char *filename,
		int type,
		std::set<int> &shaderlist,
		const char *localSourceAddition,
		int uniformBlockCount,
		UniformBlock **uniformBlocks);

	bool insertDefines(
		const char *localSourceAddition,
		int uniformBlockCount,
		UniformBlock **uniformBlocks);

#ifdef SHADER_ENABLE_UB
	void setUniformBufferSlots(int program_num);
#endif
	void setSamplerBindings(int program_num);

#ifdef SHADER_RELOAD
	void *updateThreadHandle;
	CriticalSection updateCS;
	unsigned int shaderUpdateList[maxShaders];
	unsigned int shaderUpdateCount;
#endif
	bool updateShader(int shader_num, std::set<int> &relink_programs);
	void relinkProgram(int program_num);

	void updateAllShaders();
};

#endif
