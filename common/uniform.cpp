#include "uniform.h"
#include "render_includes.h"
#include <string.h>

//#define USE_MAP_BUFFER_RANGE

UniformBlock::UniformBlock(unsigned int size)
: _size(size)
, _firstUpload(true)
{
	_data = new unsigned char [_size];
	_dataPrev = new unsigned char [_size];

	glGenBuffers(1, &_handle);
#if defined(USE_MAP_BUFFER_RANGE)
	glBindBuffer(GL_UNIFORM_BUFFER, _handle);
	glBufferData(GL_UNIFORM_BUFFER, _size, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
#endif
}

UniformBlock::~UniformBlock()
{
	delete [] _data;
	_data = 0;

	delete [] _dataPrev;
	_dataPrev = 0;

	if (_handle)
		glDeleteBuffers(1, &_handle);
	_handle = 0;
}

void UniformBlock::uploadData()
{
	if (_firstUpload)
		_firstUpload = false;
	else
	{
		if (memcmp(_dataPrev, _data, _size) == 0)
			return; // data is unchanged
	}

#if defined(USE_MAP_BUFFER_RANGE)
	glBindBuffer(GL_UNIFORM_BUFFER, _handle);
	void *bufferData = glMapBufferRange(GL_UNIFORM_BUFFER, 0, _size,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	if (bufferData)
	{
		memcpy(bufferData, _data, _size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
#else
	glBindBuffer(GL_UNIFORM_BUFFER, _handle);
	glBufferData(GL_UNIFORM_BUFFER, _size, _data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
#endif

	memcpy(_dataPrev, _data, _size);
}
