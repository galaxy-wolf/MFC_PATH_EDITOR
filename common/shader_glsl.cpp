#define _CRT_SECURE_NO_WARNINGS
#include "shader_glsl.h"
#include "platform.h"
#include "uniform.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#if defined(COMPILER_VC)
# define strnicmp _strnicmp
# define snprintf _snprintf
#else
# include <strings.h>
# define stricmp strncasecmp
#endif

#ifdef SHADER_RELOAD
# undef APIENTRY
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
DWORD WINAPI ShaderUpdateCheckThread(void *lpParameter);
#endif

#define LOG_NAME "shaderlog_glsl.txt"


int GLSLProgram::getUniformLocation(const char *name) const
{
	FixedString<maxUniformName> nameStr = name;

	std::map<FixedString<maxUniformName>, int>::iterator it =
		uniformLocations.find(nameStr);

	if (it != uniformLocations.end())
	{
		return it->second;
	}
	else
	{
		// need to look it up
		int location = glGetUniformLocation(handle, nameStr.c_str());
		uniformLocations[nameStr] = location;
		return location;
	}
}

bool GLSLShaderManager::updateShader(int shader_num, std::set<int> &relink_programs)
{
	char fullname[256];
	switch (shader[shader_num].type)
	{
	case Shader::type_vertex:
		sprintf(fullname, "%s/%s.shv", shaderPath.c_str(), shader[shader_num].name);
		break;
	case Shader::type_tess_control:
		sprintf(fullname, "%s/%s.shc", shaderPath.c_str(), shader[shader_num].name);
		break;
	case Shader::type_tess_eval:
		sprintf(fullname, "%s/%s.she", shaderPath.c_str(), shader[shader_num].name);
		break;
	case Shader::type_geometry:
		sprintf(fullname, "%s/%s.shg", shaderPath.c_str(), shader[shader_num].name);
		break;
	case Shader::type_fragment:
		sprintf(fullname, "%s/%s.shf", shaderPath.c_str(), shader[shader_num].name);
		break;
	case Shader::type_compute:
		sprintf(fullname, "%s/%s.shx", shaderPath.c_str(), shader[shader_num].name);
		break;
	default:
		writeLog("Updating Shader Failed: Unknown shader type\n");
		return false;
	}

	writeLog("\n\nUpdating Shader:	%s\n", fullname);


	FILE *file;
	int rval;

	file = fopen(fullname, "rb");
	if (!file)
	{
		writeLog("	Failed to load file: %s\n", fullname);
		return false;
	}
	fseek(file, 0, SEEK_END);
	int shaderLen = (int)ftell(file);
	fseek(file, 0, SEEK_SET);

	if (shaderLen >= maxShaderLen - 1)
	{
		writeLog("	File %s too large\n", fullname);
		return false;
	}

	fread(shaderBuffer, shaderLen, 1, file);
	shaderBuffer[shaderLen] = 0; // null terminate
	fclose(file);

	if (!insertDefines(
		shader[shader_num].localSourceAddition,
#ifdef SHADER_ENABLE_UB
		shader[shader_num].uniformBlocks,
		shader[shader_num].uniformBlock
#else
		0, 0
#endif
		))
	{
		writeLog("	Failed to insert #defines: %s\n", fullname);
		return false;
	}
	shaderLen = (int)strlen(shaderBuffer);

	// source, compile, and check for errors
	char *src = shaderBuffer;
	glShaderSource(shader[shader_num].id, 1, (const GLchar**)&src, &shaderLen);
	glCompileShader(shader[shader_num].id);
	glGetShaderiv(shader[shader_num].id, GL_COMPILE_STATUS, &rval);
	if (rval)
		writeLog("	Shader %s compile: success\n", fullname);
	else
		writeLog("	Shader %s compile: fail\n", fullname);
	glGetShaderInfoLog(shader[shader_num].id, maxInfoLogLen, &rval, infoLogBuffer);
	writeLog("	Info Log:\n%s\n\n", infoLogBuffer);


	relink_programs.insert(shader[shader_num].dependent_programs.begin(), shader[shader_num].dependent_programs.end());

	return true;
}

bool GLSLShaderManager::insertDefines(
	const char *localSourceAddition,
	int uniformBlockCount,
	UniformBlock **uniformBlocks)
{
	// insert defines after #version
	bool versionPresent = false;
	char *p = strstr(shaderBuffer, "#version");
	if (!p)
		p = shaderBuffer;
	else
	{
		versionPresent = true;

		while (*p != 0 && *p != '\n')
			p++;
		p++;
	}

	// build #define strings
	int defineSize = 0;
	char defineBuffer[maxShaderLen] = {};
#if !defined(PLATFORM_IPHONE)
	if (!versionPresent)
	{
		// default to version 120 on desktop to keep compatibility with GLES shaders (GLES #version 100 is equivalent to desktop #version 120)
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "#version 120\n");
	}
#endif
	{
		// make the ATI GLSL compiler shut up
		//defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "precision highp float;\n");
		//defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "precision highp int;\n");
	}
#if !defined(PLATFORM_IPHONE)
	{
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "#define highp \n");
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "#define mediump \n");
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "#define lowp \n");
	}
#endif
	if (localSourceAddition)
	{
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "%s\n", localSourceAddition);
	}
	for (std::set<FixedString<maxDefineLen> >::iterator it = shaderDefines.begin(); it != shaderDefines.end(); ++it)
	{
		FixedString<maxDefineLen> defineValue = *it;
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "#define %s\n", defineValue.c_str());
	}
	for (int n = 0; n < uniformBlockCount; n++)
	{
		defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "%s\n", uniformBlocks[n]->glslString());
	}
	// set line counter so the #defines don't cause bad line numbers in error reporting
	defineSize += snprintf(defineBuffer + defineSize, maxShaderLen - defineSize - 1, "#line %d\n", 2);

	// move shader source to make room for #defines
	char *p2 = p + defineSize;
	int sourceLen = (int)strlen(p);
	if (p2 + sourceLen > shaderBuffer + maxShaderLen - 1)
		return false; // not enough room
	memmove(p2, p, sourceLen);
	*(p2 + sourceLen) = 0;

	// copy in #defines
	memcpy(p, defineBuffer, defineSize);

	return true;
}

bool GLSLShaderManager::loadShader(
	const char *filename,
	int type,
	std::set<int> &shaderlist,
	const char *localSourceAddition,
	int uniformBlockCount,
	UniformBlock **uniformBlocks)
{
	// has it already been loaded?
	for (unsigned int n = 0; n < shaders; n++)
	{
		if (strncmp(filename, shader[n].name, Shader::maxName) == 0
			&& strncmp(localSourceAddition, shader[n].localSourceAddition, Shader::maxLocalSourceAddition) == 0
			&& type == shader[n].type)
		{
			// add this to the list of shaders to be linked
			shaderlist.insert(n);
			return true;
		}
	}

	char fullname[256];
	unsigned int shaderid;
	switch (type)
	{
	case Shader::type_vertex:
		sprintf(fullname, "%s/%s.shv", shaderPath.c_str(), filename);
		shaderid = glCreateShader(GL_VERTEX_SHADER);
		break;
#ifdef SHADER_ENABLE_TS			
	case Shader::type_tess_control:
		sprintf(fullname, "%s/%s.shc", shaderPath.c_str(), filename);
		shaderid = glCreateShader(GL_TESS_CONTROL_SHADER);
		break;
	case Shader::type_tess_eval:
		sprintf(fullname, "%s/%s.she", shaderPath.c_str(), filename);
		shaderid = glCreateShader(GL_TESS_EVALUATION_SHADER);
		break;
#endif
#ifdef SHADER_ENABLE_GS
	case Shader::type_geometry:
		sprintf(fullname, "%s/%s.shg", shaderPath.c_str(), filename);
		shaderid = glCreateShader(GL_GEOMETRY_SHADER_ARB);
		break;
#endif
	case Shader::type_fragment:
		sprintf(fullname, "%s/%s.shf", shaderPath.c_str(), filename);
		shaderid = glCreateShader(GL_FRAGMENT_SHADER);
		break;
#ifdef SHADER_ENABLE_CS
	case Shader::type_compute:
		sprintf(fullname, "%s/%s.shx", shaderPath.c_str(), filename);
		shaderid = glCreateShader(GL_COMPUTE_SHADER);
		break;
#endif
	default:
		writeLog("	Unknown shader type\n");
		return false;
	}


	FILE *file;
	int rval;

	file = fopen(fullname, "rb");
	if (!file)
	{
		writeLog("	Failed to load file: %s\n", fullname);
		return false;
	}
	fseek(file, 0, SEEK_END);
	int shaderLen = (int)ftell(file);
	fseek(file, 0, SEEK_SET);

	if (shaderLen >= maxShaderLen - 1)
	{
		writeLog("	File %s too large\n", fullname);
		fclose(file);
		return false;
	}

	fread(shaderBuffer, shaderLen, 1, file);
	shaderBuffer[shaderLen] = 0; // null terminate
	fclose(file);

	if (!insertDefines(localSourceAddition, uniformBlockCount, uniformBlocks))
	{
		writeLog("	Failed to insert #defines: %s\n", fullname);
		return false;
	}
	shaderLen = (int)strlen(shaderBuffer);

	// source, compile, and check for errors
	char *src = shaderBuffer;
	glShaderSource(shaderid, 1, (const GLchar**)&src, &shaderLen);
	glCompileShader(shaderid);
	glGetShaderiv(shaderid, GL_COMPILE_STATUS, &rval);
	if (rval)
		writeLog("	Shader %s compile: success\n", fullname);
	else
		writeLog("	Shader %s compile: fail\n", fullname);
	glGetShaderInfoLog(shaderid, maxInfoLogLen, &rval, infoLogBuffer);
	writeLog("	Info Log:\n%s\n", infoLogBuffer);



	Shader newshader;

	newshader.id = shaderid;
	newshader.type = type;
	strcpy(newshader.name, filename);
	strcpy(newshader.localSourceAddition, localSourceAddition);
#ifdef SHADER_ENABLE_UB
	newshader.uniformBlocks = uniformBlockCount;
	for (int n = 0; n < uniformBlockCount; n++)
	{
		newshader.uniformBlock[n] = uniformBlocks[n];
	}
#endif

	shader[shaders] = newshader;
	// add this to the list of shaders to be linked
	shaderlist.insert(shaders);
	shaders++;

	return true;
}

#ifdef SHADER_ENABLE_UB
void GLSLShaderManager::setUniformBufferSlots(int program_num)
{
	if (!_uniformBuffersSupported)
		return;

	for (int n = 0; n < program[program_num].uniformBlocks; n++)
	{
		UniformBlock *ub = program[program_num].uniformBlock[n];

		unsigned int uniformBlockIndex = glGetUniformBlockIndex(program[program_num].handle, ub->name());
		if (GL_INVALID_INDEX != uniformBlockIndex)
		{
			glUniformBlockBinding(program[program_num].handle, uniformBlockIndex, n);
		}
	}
}
#endif

void GLSLShaderManager::setSamplerBindings(int program_num)
{
	int currentProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	glUseProgram(program[program_num].handle);

	for (int n = 0; n < _maxTextureImageUnits; n++)
	{
		char samplerName[128];
		sprintf(samplerName, "sampler%d", n);

		glUniform1i(program[program_num].getUniformLocation(samplerName), n);
	}

	glUseProgram(currentProgram);
}

void GLSLShaderManager::relinkProgram(int program_num)
{
	writeLog(
		"Relinking Program:"
		"	%s.shv"
#ifdef SHADER_ENABLE_TS
		"	%s.shc"
		"	%s.she"
#endif
#ifdef SHADER_ENABLE_GS
		"	%s.shg"
#endif
		"	%s.shf"
		"\n"
		"	local source addition:\n%s\n\n"
		, program[program_num].vertexShader
#ifdef SHADER_ENABLE_TS
		, program[program_num].tessControlShader
		, program[program_num].tessEvalShader
#endif
#ifdef SHADER_ENABLE_GS
		, program[program_num].geometryShader
#endif
		, program[program_num].fragmentShader
		, program[program_num].localSourceAddition
		);

	int rval;

	program[program_num].onRelink();
	glLinkProgram(program[program_num].handle);
	glGetProgramiv(program[program_num].handle, GL_LINK_STATUS, &rval);
	// need to include the attached shader names
	if (rval)
		writeLog("	Program link: success\n");
	else
		writeLog("	Program link: fail\n");
	glGetProgramInfoLog(program[program_num].handle, maxInfoLogLen, &rval, infoLogBuffer);
	writeLog("	Info Log:\n%s\n\n", infoLogBuffer);

#ifdef SHADER_ENABLE_UB	
	setUniformBufferSlots(program_num);
#endif
	setSamplerBindings(program_num);
}

const GLSLProgram* GLSLShaderManager::loadProgram(
	const char *vertexShader
	, const char *tessControlShader
	, const char *tessEvalShader
	, const char *geometryShader
	, const char *fragmentShader
	, const char *localSourceAddition
	, int uniformBlockCount
	, UniformBlock **uniformBlocks
	, GLenum gs_in
	, GLenum gs_out
	, GLenum gs_outmax
	)
{
	LoadOptions options;
	options.vertexShader = vertexShader;
	options.tessControlShader = tessControlShader;
	options.tessEvalShader = tessEvalShader;
	options.geometryShader = geometryShader;
	options.fragmentShader = fragmentShader;
	options.localSourceAddition = localSourceAddition;
	options.uniformBlockCount = uniformBlockCount;
	options.uniformBlocks = uniformBlocks;
	options.gs_in = gs_in;
	options.gs_out = gs_out;
	options.gs_outmax = gs_outmax;

	return loadProgram(options);
}

const GLSLProgram* GLSLShaderManager::loadProgramCompute(
	const char *computeShader
	, const char *localSourceAddition
	, int uniformBlockCount
	, UniformBlock **uniformBlocks
	)
{
	LoadOptions options;
	options.computeShader = computeShader;
	options.localSourceAddition = localSourceAddition;
	options.uniformBlockCount = uniformBlockCount;
	options.uniformBlocks = uniformBlocks;

	return loadProgram(options);
}

const GLSLProgram* GLSLShaderManager::loadProgram(LoadOptions options)
{
#ifdef SHADER_ENABLE_UB
	if (options.uniformBlockCount > GLSLProgram::maxUniformBlocks)
	{
		assert(0);
		return 0;
	}
#endif

	if (options.vertexShader == 0)
		options.vertexShader = "";
	bool vertexShaderEnabled = strlen(options.vertexShader) > 0;
	if (options.tessControlShader == 0)
		options.tessControlShader = "";
	bool tessControlShaderEnabled = strlen(options.tessControlShader) > 0;
	if (options.tessEvalShader == 0)
		options.tessEvalShader = "";
	bool tessEvalShaderEnabled = strlen(options.tessEvalShader) > 0;
	if (options.geometryShader == 0)
		options.geometryShader = "";
	bool geometryShaderEnabled = strlen(options.geometryShader) > 0;
	if (options.fragmentShader == 0)
		options.fragmentShader = "";
	bool fragmentShaderEnabled = strlen(options.fragmentShader) > 0;
	if (options.computeShader == 0)
		options.computeShader = "";
	bool computeShaderEnabled = strlen(options.computeShader) > 0;

	if (options.localSourceAddition == 0)
	{
		options.localSourceAddition = "";
	}
	else if (strlen(options.localSourceAddition) > GLSLProgram::maxLocalSourceAddition - 1)
	{
		assert(0);
		return 0;
	}

	for (unsigned int n = 0; n < programs; n++)
	{
		if (strncmp(options.vertexShader, program[n].vertexShader, GLSLProgram::maxName) == 0
#ifdef SHADER_ENABLE_TS
			&& strncmp(options.tessControlShader, program[n].tessControlShader, GLSLProgram::maxName) == 0
			&& strncmp(options.tessEvalShader, program[n].tessEvalShader, GLSLProgram::maxName) == 0
#endif
#ifdef SHADER_ENABLE_GS
			&& strncmp(options.geometryShader, program[n].geometryShader, GLSLProgram::maxName) == 0
#endif
			&& strncmp(options.fragmentShader, program[n].fragmentShader, GLSLProgram::maxName) == 0
#ifdef SHADER_ENABLE_CS
			&& strncmp(options.computeShader, program[n].computeShader, GLSLProgram::maxName) == 0
#endif
			&& strncmp(options.localSourceAddition, program[n].localSourceAddition, GLSLProgram::maxLocalSourceAddition) == 0
			)
		{
			return program + n;
		}
	}

	writeLog("\n\nCreating Program:"
		"	%s.shv"
		"	%s.shc"
		"	%s.she"
		"	%s.shg"
		"	%s.shf"
		"	%s.shx"
		"\n"
		"	local source addition:\n%s\n\n"
		, options.vertexShader
		, options.tessControlShader
		, options.tessEvalShader
		, options.geometryShader
		, options.fragmentShader
		, options.computeShader
		, options.localSourceAddition
		);


	// get all shaders required...
	std::set<int> shaderlist;
	if (vertexShaderEnabled)
	{
		writeLog("	Vertex:\n");
		if (!loadShader(options.vertexShader, Shader::type_vertex, shaderlist, options.localSourceAddition, options.uniformBlockCount, options.uniformBlocks))
		{
			writeLog("	Unable to load all required shaders\n");
			assert(0);
			return 0;
		}
	}
	if (tessControlShaderEnabled)
	{
		writeLog("	Tess Control:\n");
		if (!loadShader(options.tessControlShader, Shader::type_tess_control, shaderlist, options.localSourceAddition, options.uniformBlockCount, options.uniformBlocks))
		{
			writeLog("	Unable to load all required shaders\n");
			assert(0);
			return 0;
		}
	}
	if (tessEvalShaderEnabled)
	{
		writeLog("	Tess Eval:\n");
		if (!loadShader(options.tessEvalShader, Shader::type_tess_eval, shaderlist, options.localSourceAddition, options.uniformBlockCount, options.uniformBlocks))
		{
			writeLog("	Unable to load all required shaders\n");
			assert(0);
			return 0;
		}
	}
	if (geometryShaderEnabled)
	{
		writeLog("	Geometry:\n");
		if (!loadShader(options.geometryShader, Shader::type_geometry, shaderlist, options.localSourceAddition, options.uniformBlockCount, options.uniformBlocks))
		{
			writeLog("	Unable to load all required shaders\n");
			assert(0);
			return 0;
		}
	}
	if (fragmentShaderEnabled)
	{
		writeLog("	Fragment:\n");
		if (!loadShader(options.fragmentShader, Shader::type_fragment, shaderlist, options.localSourceAddition, options.uniformBlockCount, options.uniformBlocks))
		{
			writeLog("	Unable to load all required shaders\n");
			assert(0);
			return 0;
		}
	}
	if (computeShaderEnabled)
	{
		writeLog("	Compute:\n");
		if (!loadShader(options.computeShader, Shader::type_compute, shaderlist, options.localSourceAddition, options.uniformBlockCount, options.uniformBlocks))
		{
			writeLog("	Unable to load all required shaders\n");
			assert(0);
			return 0;
		}
	}


	// link program
	int rval;
	unsigned int programid = glCreateProgram();

#ifdef SHADER_ENABLE_GS
	if (geometryShaderEnabled)
	{
		glProgramParameteriEXT(programid, GL_GEOMETRY_INPUT_TYPE_EXT, options.gs_in);
		glProgramParameteriEXT(programid, GL_GEOMETRY_OUTPUT_TYPE_EXT, options.gs_out);
		glProgramParameteriEXT(programid, GL_GEOMETRY_VERTICES_OUT_EXT, options.gs_outmax);
	}
#endif

	// attached shaders do not need to be compiled or even supplied a source string at this point
	for (std::set<int>::iterator it = shaderlist.begin(); it != shaderlist.end(); ++it)
	{
		glAttachShader(programid, shader[*it].id);
	}

	// set up attribute bindings in advance
	if (vertexShaderEnabled)
	{
		for (int n = 0; n < _maxVertexAttribs; n++)
		{
			char attribName[32];
			sprintf(attribName, "a%d", n);
			glBindAttribLocation(programid, n, attribName);
		}
	}
	if (fragmentShaderEnabled)
	{
#if !defined(RENDER_GLES2)
		if (_shadingLanguageVersion >= 130)
		{
			for (int n = 0; n < _maxDrawBuffers; n++)
			{
				char outName[32];
				sprintf(outName, "fragDataOut%d", n);
				glBindFragDataLocation(programid, n, outName);
			}
		}
#endif
	}

	// attached shaders must be compiled
	// subsequent modifications and recompiling of shaders will not affect the linked program
	// if the currently active program is relinked, it will remain active using the newly linked code
	// relinking a program resets uniform values
	program[programs].onRelink();
	glLinkProgram(programid);
	glGetProgramiv(programid, GL_LINK_STATUS, &rval);
	// need to include the attached shader names
	if (rval)
		writeLog("	Program link: success\n");
	else
		writeLog("	Program link: fail\n");
	glGetProgramInfoLog(programid, maxInfoLogLen, &rval, infoLogBuffer);
	writeLog("	Info Log:\n%s\n\n", infoLogBuffer);


	// add it to the list of programs
	program[programs].handle = programid;
	strcpy(program[programs].vertexShader, options.vertexShader);
#ifdef SHADER_ENABLE_TS
	strcpy(program[programs].tessControlShader, options.tessControlShader);
	strcpy(program[programs].tessEvalShader, options.tessEvalShader);
#endif
#ifdef SHADER_ENABLE_GS
	strcpy(program[programs].geometryShader, options.geometryShader);
#endif
	strcpy(program[programs].fragmentShader, options.fragmentShader);
#ifdef SHADER_ENABLE_CS
	strcpy(program[programs].computeShader, options.computeShader);
#endif
	strcpy(program[programs].localSourceAddition, options.localSourceAddition);
#ifdef SHADER_ENABLE_UB
	program[programs].uniformBlocks = options.uniformBlockCount;
	for (int n = 0; n < options.uniformBlockCount; n++)
	{
		program[programs].uniformBlock[n] = options.uniformBlocks[n];
	}
#endif
	int program_num = programs++;

	// update dependencies
	for (std::set<int>::iterator it = shaderlist.begin(); it != shaderlist.end(); ++it)
	{
		shader[*it].dependent_programs.insert(program_num);
	}

#ifdef SHADER_ENABLE_UB
	setUniformBufferSlots(program_num);
#endif
	setSamplerBindings(program_num);

	return program + program_num;
}

void GLSLShaderManager::addDefine(const char *name)
{
	std::pair<std::set<FixedString<maxDefineLen> >::iterator, bool> rval;
	rval = shaderDefines.insert((const FixedString<maxDefineLen>)(name));
	if (rval.second)
	{
		updateAllShaders();
	}
}

void GLSLShaderManager::removeDefine(const char *name)
{
	if (shaderDefines.erase((const FixedString<maxDefineLen>)(name)) > 0)
	{
		updateAllShaders();
	}
}

void GLSLShaderManager::updateAllShaders()
{
	std::set<int> relink_programs;
	for (unsigned int n = 0; n < shaders; n++)
		updateShader(n, relink_programs);

	for (std::set<int>::iterator it = relink_programs.begin(); it != relink_programs.end(); ++it)
	{
		relinkProgram(*it);
	}
}

GLSLShaderManager::GLSLShaderManager(const char *_shaderPath)
{
	shaderPath = _shaderPath;
	programs = 0;
	shaders = 0;

	logFile = 0;

#ifdef SHADER_RELOAD
	shaderUpdateCount = 0;
	updateThreadHandle = 0;
#endif
}

GLSLShaderManager::~GLSLShaderManager()
{
	shutdown();
}

void GLSLShaderManager::writeLogV(const char *format, va_list argptr)
{
	enum {bufferLen = 4096};
	char buffer[bufferLen];
	vsnprintf(buffer, bufferLen - 1, format, argptr);
	buffer[bufferLen - 1] = 0;

//	printf("%s", buffer);

	if (logFile)
	{
		fprintf((FILE*)logFile, "%s", buffer);
		fflush((FILE*)logFile);
	}
}

void GLSLShaderManager::writeLog(const char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	writeLogV(format, argptr);
	va_end(argptr);
}

bool GLSLShaderManager::startup()
{
	// try the local directory
	if (!logFile)
	{
		logFile = fopen(LOG_NAME, "wb");
	}

	{
#if defined(RENDER_GLES2)
		_shadingLanguageVersion = 120;
#else
		const char *shadingLanguageVersionStr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		if (shadingLanguageVersionStr)
		{
			writeLog("Shading Language Version: %s\n\n", shadingLanguageVersionStr);
		}
		else
		{
			writeLog("Unable to get shading language version\n\n");
			return false;
		}
		int shadingLanguageVersionMajor = 0;
		int shadingLanguageVersionMinor = 0;
		sscanf(shadingLanguageVersionStr, "%d.%d", &shadingLanguageVersionMajor, &shadingLanguageVersionMinor);
		_shadingLanguageVersion = shadingLanguageVersionMajor * 100 + shadingLanguageVersionMinor;
#endif
#if !defined(RENDER_GLES2)
		if (_shadingLanguageVersion < 120)
		{
			writeLog("Unsupported shading language version\n\n");
			return false;
		}
#endif
		
#if !defined(RENDER_GLES2)
		const char *extensionString = (const char*)glGetString(GL_EXTENSIONS);
		if (!extensionString)
		{
			writeLog("Failed to get GL extension string\n\n");
			return false;
		}
		
		if (strstr(extensionString, "GL_ARB_fragment_shader"))
		{
			int maxFragmentUniformComponents = -1;
			int maxTextureCoords = -1;
			_maxTextureImageUnits = -1;
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);
			glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoords);
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTextureImageUnits);
			writeLog("GL_ARB_fragment_shader found\n");
			writeLog("Max Fragment Uniform Components: %d\n", maxFragmentUniformComponents);
			writeLog("Max Texture Coords: %d\n", maxTextureCoords);
			writeLog("Max Texture Image Units: %d\n", _maxTextureImageUnits);
			writeLog("\n");
		}
		else
		{
			_maxTextureImageUnits = 0;
			writeLog("GL_ARB_fragment_shader missing\n\n");
			return false;
		}

		if (strstr(extensionString, "GL_ARB_vertex_shader"))
		{
			int maxVertexUniformComponents = -1;
			int maxVaryingFloats = -1;
			_maxVertexAttribs = -1;
			int maxTextureImageUnits = -1;
			int maxVertexTextureImageUnits = -1;
			int maxCombinedTextureImageUnits = -1;
			int maxTextureCoords = -1;
			glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
			glGetIntegerv(GL_MAX_VARYING_FLOATS, &maxVaryingFloats);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_maxVertexAttribs);
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);
			glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureImageUnits);
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits);
			glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoords);
			writeLog("GL_ARB_vertex_shader found\n");
			writeLog("Max Vertex Uniform Components: %d\n", maxVertexUniformComponents);
			writeLog("Max Varying Floats: %d\n", maxVaryingFloats);
			writeLog("Max Vertex Attribs: %d\n", _maxVertexAttribs);
			writeLog("Max Texture Image Units: %d\n", maxTextureImageUnits);
			writeLog("Max Vertex Texture Image Units: %d\n", maxVertexTextureImageUnits);
			writeLog("Max Combined Texture Image Units: %d\n", maxCombinedTextureImageUnits);
			writeLog("Max Texture Coords: %d\n", maxTextureCoords);
			writeLog("\n");
		}
		else
		{
			_maxVertexAttribs = 0;
			writeLog("GL_ARB_vertex_shader missing\n\n");
			return false;
		}

#ifdef SHADER_ENABLE_GS
		if (strstr(extensionString, "GL_ARB_geometry_shader4"))
		{
			int maxGeometryTextureImageUnits = -1;
			int maxGeometryVaryingComponents = -1;
			int maxVertexVaryingComponents = -1;
			int maxVaryingComponents = -1;
			int maxGeometryUniformComponents = -1;
			int maxGeometryOutputVertices = -1;
			int maxGeometryTotalOutputComponents = -1;
			glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB, &maxGeometryTextureImageUnits);
			glGetIntegerv(GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB, &maxGeometryVaryingComponents);
			glGetIntegerv(GL_MAX_VERTEX_VARYING_COMPONENTS_ARB, &maxVertexVaryingComponents);
			glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &maxVaryingComponents);
			glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB, &maxGeometryUniformComponents);
			glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB, &maxGeometryOutputVertices);
			glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB, &maxGeometryTotalOutputComponents);
			writeLog("GL_ARB_geometry_shader4 found\n");
			writeLog("Max Geometry Texture Image Units: %d\n", maxGeometryTextureImageUnits);
			writeLog("Max Geometry Varying Components: %d\n", maxGeometryVaryingComponents);
			writeLog("Max Vertex Varying Components: %d\n", maxVertexVaryingComponents);
			writeLog("Max Varying Components: %d\n", maxVaryingComponents);
			writeLog("Max Geometry Uniform Components: %d\n", maxGeometryUniformComponents);
			writeLog("Max Geometry Output Vertices: %d\n", maxGeometryOutputVertices);
			writeLog("Max Geometry Total Output Components: %d\n", maxGeometryTotalOutputComponents);
			writeLog("\n");
		}
		else
		{
			writeLog("GL_ARB_geometry_shader4 missing\n\n");
		}
#endif

		if (strstr(extensionString, "GL_EXT_gpu_shader4"))
		{
			int minProgramTexelOffset = -1;
			int maxProgramTexelOffset = -1;
			glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &minProgramTexelOffset);
			glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &maxProgramTexelOffset);
			writeLog("GL_EXT_gpu_shader4 found\n");
			writeLog("Min Program Texel Offset: %d\n", minProgramTexelOffset);
			writeLog("Max Program Texel Offset: %d\n", maxProgramTexelOffset);
			writeLog("\n");
		}
		else
		{
			writeLog("GL_EXT_gpu_shader4 missing\n\n");
		}

		if (strstr(extensionString, "GL_ARB_draw_buffers"))
		{
			_maxDrawBuffers = -1;
			glGetIntegerv(GL_MAX_DRAW_BUFFERS, &_maxDrawBuffers);
			writeLog("GL_ARB_draw_buffers found\n");
			writeLog("Max Draw Buffers: %d\n", _maxDrawBuffers);
			writeLog("\n");
		}
		else
		{
			_maxDrawBuffers = 1;
			writeLog("GL_ARB_draw_buffers missing\n\n");
		}

		if (strstr(extensionString, "GL_ARB_uniform_buffer_object"))
		{
			_uniformBuffersSupported = true;
			writeLog("GL_ARB_uniform_buffer_object found\n");
			writeLog("\n");
		}
		else
		{
			_uniformBuffersSupported = false;
			writeLog("GL_ARB_uniform_buffer_object missing\n\n");
		}

#ifdef SHADER_ENABLE_TS
		if (strstr(extensionString, "GL_ARB_tessellation_shader"))
		{
			int maxPatchVertices = -1;
			int maxTessGenLevel = -1;
			int maxTessControlTextureImageUnits = -1;
			int maxTessEvaluationTextureImageUnits = -1;
			int maxTessControlOutputComponents = -1;
			int maxTessPatchComponents = -1;
			int maxTessControlTotalOutputComponents = -1;
			int maxTessEvaluationOutputComponents = -1;
			int maxTessControlInputComponents = -1;
			int maxTessEvaluationInputComponents = -1;
			glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
			glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel);
			glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, &maxTessControlTextureImageUnits);
			glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, &maxTessEvaluationTextureImageUnits);
			glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &maxTessControlOutputComponents);
			glGetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS, &maxTessPatchComponents);
			glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &maxTessControlTotalOutputComponents);
			glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &maxTessEvaluationOutputComponents);
			glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &maxTessControlInputComponents);
			glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &maxTessEvaluationInputComponents);
			writeLog("GL_ARB_tessellation_shader found\n");
			writeLog("Max Patch Vertices: %d\n", maxPatchVertices);
			writeLog("Max Tess Gen Level: %d\n", maxTessGenLevel);
			writeLog("Max Tess Control Texture Image Units: %d\n", maxTessControlTextureImageUnits);
			writeLog("Max Tess Evaluation Texture Image Units: %d\n", maxTessEvaluationTextureImageUnits);
			writeLog("Max Tess Control Output Components: %d\n", maxTessControlOutputComponents);
			writeLog("Max Tess Patch Components: %d\n", maxTessPatchComponents);
			writeLog("Max Tess Control Total Output Components: %d\n", maxTessControlTotalOutputComponents);
			writeLog("Max Tess Evaluation Output Components: %d\n", maxTessEvaluationOutputComponents);
			writeLog("Max Tess Control Input Components: %d\n", maxTessControlInputComponents);
			writeLog("Max Tess Evaluation Input Components: %d\n", maxTessEvaluationInputComponents);
			writeLog("\n");
		}
		else
		{
			writeLog("GL_ARB_tessellation_shader missing\n\n");
		}
#endif

#ifdef SHADER_ENABLE_CS
		if (strstr(extensionString, "GL_ARB_compute_shader"))
		{
			int maxComputeUniformBlocks = -1;
			int maxComputeUniformComponents = -1;
			int maxCombinedComputeUniformComponents = -1;
			int maxComputeTextureImageUnits = -1;
			int maxComputeImageUniforms = -1;
			int maxComputeSharedMemorySize = -1;
			int maxComputeAtomicCounterBuffers = -1;
			int maxComputeAtomicCounters = -1;
			int maxComputeWorkGroupInvocations = -1;
			int maxComputeWorkGroupCount[3] = {-1, -1, -1};
			int maxComputeWorkGroupSize[3] = {-1, -1, -1};
			glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &maxComputeUniformBlocks);
			glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &maxComputeUniformComponents);
			glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &maxCombinedComputeUniformComponents);
			glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &maxComputeTextureImageUnits);
			glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, &maxComputeImageUniforms);
			glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &maxComputeSharedMemorySize);
			glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &maxComputeAtomicCounterBuffers);
			glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &maxComputeAtomicCounters);
			glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxComputeWorkGroupInvocations);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxComputeWorkGroupCount[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxComputeWorkGroupCount[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxComputeWorkGroupCount[2]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxComputeWorkGroupSize[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxComputeWorkGroupSize[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxComputeWorkGroupSize[2]);
			writeLog("GL_ARB_compute_shader found\n");
			writeLog("Max Compute Uniform Blocks: %d\n", maxComputeUniformBlocks);
			writeLog("Max Compute Uniform Components: %d\n", maxComputeUniformComponents);
			writeLog("Max Combined Compute Uniform Components: %d\n", maxCombinedComputeUniformComponents);
			writeLog("Max Compute Texture Image Units: %d\n", maxComputeTextureImageUnits);
			writeLog("Max Compute Image Uniforms: %d\n", maxComputeImageUniforms);
			writeLog("Max Compute Shared Memory Size: %d\n", maxComputeSharedMemorySize);
			writeLog("Max Compute Atomic Counter Buffers: %d\n", maxComputeAtomicCounterBuffers);
			writeLog("Max Compute Atomic Counters: %d\n", maxComputeAtomicCounters);
			writeLog("Max Compute Work Group Invocations: %d\n", maxComputeWorkGroupInvocations);
			writeLog("Max Compute Work Group Count: %d %d %d\n", maxComputeWorkGroupCount[0], maxComputeWorkGroupCount[1], maxComputeWorkGroupCount[2]);
			writeLog("Max Compute Work Group Size: %d %d %d\n", maxComputeWorkGroupSize[0], maxComputeWorkGroupSize[1], maxComputeWorkGroupSize[2]);
			writeLog("\n");
		}
		else
		{
			writeLog("GL_ARB_compute_shader missing\n\n");
		}
#endif
		
#endif
	}

#ifdef SHADER_RELOAD
	DWORD threadid;
	updateThreadHandle = CreateThread(0, 0, ShaderUpdateCheckThread, this, 0, &threadid);
	SetThreadPriority(updateThreadHandle, -1);
#endif

	return true;
}

void GLSLShaderManager::shutdown()
{
	for (unsigned int n = 0; n < programs; n++)
	{
		if (program[n].handle)
			glDeleteProgram(program[n].handle);
		program[n].handle = 0;
	}
	programs = 0;
	
	for (unsigned int n = 0; n < shaders; n++)
	{
		if (shader[n].id)
			glDeleteShader(shader[n].id);
		shader[n].id = 0;
	}
	shaders = 0;

#ifdef SHADER_RELOAD
	if (updateThreadHandle)
	{
		TerminateThread(updateThreadHandle, 0);
		CloseHandle(updateThreadHandle);
		updateThreadHandle = 0;
	}
	shaderUpdateCount = 0;
#endif

	shaderDefines.clear();

	if (logFile)
		fclose((FILE*)logFile);
	logFile = 0;
}

#ifdef SHADER_RELOAD
static DWORD WINAPI ShaderUpdateCheckThread(void *lpParameter)
{
	return ((GLSLShaderManager*)lpParameter)->updateFileLoop();
}
int GLSLShaderManager::updateFileLoop()
{
	HANDLE hDir = CreateFileA(
		shaderPath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (hDir == INVALID_HANDLE_VALUE)
		return 0;

	unsigned char *fnbuffer[10000];
	while (1) // could check for a terminate signal...
	{
		FILE_NOTIFY_INFORMATION *fn = (FILE_NOTIFY_INFORMATION *)fnbuffer;
		int bytesreturned = 0;
		if (ReadDirectoryChangesW(hDir, (void *)fnbuffer, 10000, true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, (LPDWORD)&bytesreturned, 0, 0))
		{
			updateCS.enter();

			while (1)
			{
				if (fn->Action == FILE_ACTION_ADDED || fn->Action == FILE_ACTION_MODIFIED || fn->Action == FILE_ACTION_RENAMED_NEW_NAME)
				{
					int nameLen = fn->FileNameLength / 2;
					char *name = new char [nameLen + 1];
					memset(name, 0, nameLen + 1);
					wcstombs(name, fn->FileName, nameLen + 1);

					int type = Shader::type_none;

					for (int n = 0; n < nameLen; n++)
					{
						if (name[n] == '.') // remove extension
						{
							name[n] = 0;

							// get the type by file extension
							if (n + 3 < nameLen)
							{
								if (name[n + 3] == 'v')
									type = Shader::type_vertex;
								else if (name[n + 3] == 'c')
									type = Shader::type_tess_control;
								else if (name[n + 3] == 'e')
									type = Shader::type_tess_eval;
								else if (name[n + 3] == 'g')
									type = Shader::type_geometry;
								else if (name[n + 3] == 'f')
									type = Shader::type_fragment;
								else if (name[n + 3] == 'x')
									type = Shader::type_compute;
							}
							break;
						}
					}

					if (type != Shader::type_none)
					{
						for (unsigned int n = 0; n < shaders; n++)
						{
							if (_stricmp(name, shader[n].name) == 0
								&& type == shader[n].type)
							{
								// check for duplicates (seems to create two modified events
								unsigned int i;
								for (i = 0; i < shaderUpdateCount; i++)
								{
									if (shaderUpdateList[i] == n)
										break;
								}
								if (i >= shaderUpdateCount && shaderUpdateCount < maxShaders)
								{
									shaderUpdateList[shaderUpdateCount] = n;
									shaderUpdateCount++;
								}
								// keep searching, a shader can be loaded multiple times with different local source additions
							}
						}
					}

					delete [] name;
				}

				if (fn->NextEntryOffset == 0)
					break;
				fn = (FILE_NOTIFY_INFORMATION *)(((unsigned char *)fn) + fn->NextEntryOffset);
			}

			updateCS.leave();
		}
		else // nothing happened
		{
			Sleep(100);
		}
	}

	CloseHandle(hDir);

	return 1;
}
#endif

bool GLSLShaderManager::update()
{
#ifdef SHADER_RELOAD
	updateCS.enter();

	bool updated = false;

	if (shaderUpdateCount > 0)
	{
		// update shaders and get a list of all dependent programs that must be relinked
		std::set<int> relink_programs;
		for (unsigned int n = 0; n < shaderUpdateCount; n++)
		{
			updateShader(shaderUpdateList[n], relink_programs);
		}
		shaderUpdateCount = 0;

		for (std::set<int>::iterator it = relink_programs.begin(); it != relink_programs.end(); ++it)
		{
			relinkProgram(*it);
			updated = true;
		}
	}

	updateCS.leave();

	return updated;
#else
	return false;
#endif
}
