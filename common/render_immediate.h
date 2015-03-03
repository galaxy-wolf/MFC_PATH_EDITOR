#ifndef RENDER_IMMEDIATE_H
#define RENDER_IMMEDIATE_H

#include "platform.h"
#include "debug.h"
#include "render_includes.h"

#include <string.h>

// GL does not support quads natively, need to emulate with triangles
#if defined (RENDER_GLES2)
# define EMULATE_QUADS
#endif
// enable generic attributes
#define GENERIC_ATTRIBS

#if !defined(RENDER_GLES2)
// enable classic GL attributes
# define CLASSIC_ATTRIBS
#endif

// permutations for setAttrib
// need a unique integer for each possible combination to ensure we generate correct code for the setAttrib switch
#if   defined(CLASSIC_ATTRIBS) && defined(GENERIC_ATTRIBS)
# define GLOBAL_SET_ATTRIB_PERMUTATION 1
#elif defined(CLASSIC_ATTRIBS) && !defined(GENERIC_ATTRIBS)
# define GLOBAL_SET_ATTRIB_PERMUTATION 2
#elif !defined(CLASSIC_ATTRIBS) && defined(GENERIC_ATTRIBS)
# define GLOBAL_SET_ATTRIB_PERMUTATION 3
#elif !defined(CLASSIC_ATTRIBS) && !defined(GENERIC_ATTRIBS)
# define GLOBAL_SET_ATTRIB_PERMUTATION 4
#endif

namespace RI
{

enum
{
	prim_none = 0,
	prim_points,
	prim_line_strip,
	prim_lines,
	prim_triangle_strip,
	prim_triangle_fan,
	prim_triangles,
	prim_quads,

	// max prim type
	prim_types,

	// aliases
	prim_quad_strip = prim_triangle_strip
};

// attrib enable bits
enum
{
#ifdef CLASSIC_ATTRIBS
	attrib_position = (1 << 0),
	attrib_weight = (1 << 1),
	attrib_normal = (1 << 2),
	attrib_color = (1 << 3),
	attrib_color_secondary = (1 << 4),
	attrib_fogcoord = (1 << 5),
	attrib_classic6 = (1 << 6),
	attrib_classic7 = (1 << 7),
	attrib_texcoord0 = (1 << 8),
	attrib_texcoord1 = (1 << 9),
	attrib_texcoord2 = (1 << 10),
	attrib_texcoord3 = (1 << 11),
	attrib_texcoord4 = (1 << 12),
	attrib_texcoord5 = (1 << 13),
	attrib_texcoord6 = (1 << 14),
	attrib_texcoord7 = (1 << 15),
#endif

#ifdef GENERIC_ATTRIBS
	attrib_0 = (1 << 16),
	attrib_1 = (1 << 17),
	attrib_2 = (1 << 18),
	attrib_3 = (1 << 19),
	attrib_4 = (1 << 20),
	attrib_5 = (1 << 21),
	attrib_6 = (1 << 22),
	attrib_7 = (1 << 23),
	attrib_8 = (1 << 24),
	attrib_9 = (1 << 25),
	attrib_10 = (1 << 26),
	attrib_11 = (1 << 27),
	attrib_12 = (1 << 28),
	attrib_13 = (1 << 29),
	attrib_14 = (1 << 30),
	attrib_15 = (1 << 31),
#endif

	maxAttribs = 32
};

// index mapping for attribs
enum
{
#ifdef CLASSIC_ATTRIBS
	index_position = 0,
	index_weight = 1,
	index_normal = 2,
	index_color = 3,
	index_color_secondary = 4,
	index_fogcoord = 5,
	index_classic6 = 6,
	index_classic7 = 7,
	index_texcoord0 = 8,
	index_texcoord1 = 9,
	index_texcoord2 = 10,
	index_texcoord3 = 11,
	index_texcoord4 = 12,
	index_texcoord5 = 13,
	index_texcoord6 = 14,
	index_texcoord7 = 15,
#endif

#ifdef GENERIC_ATTRIBS
	index_0 = 16,
	index_1 = 17,
	index_2 = 18,
	index_3 = 19,
	index_4 = 20,
	index_5 = 21,
	index_6 = 22,
	index_7 = 23,
	index_8 = 24,
	index_9 = 25,
	index_10 = 26,
	index_11 = 27,
	index_12 = 28,
	index_13 = 29,
	index_14 = 30,
	index_15 = 31,
#endif
};

class AttribValue
{
public:

	float x, y, z, w;

	// too slow to initialize to defaults
	AttribValue()
	{
	}
	//AttribValue() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
	//{
	//}

	AttribValue(float _x) : x(_x), y(0.0f), z(0.0f), w(1.0f)
	{
	}

	AttribValue(float _x, float _y) : x(_x), y(_y), z(0.0f), w(1.0f)
	{
	}

	AttribValue(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(1.0f)
	{
	}

	AttribValue(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
	{
	}
};

template <unsigned int AttribBufferSize, unsigned int Attribs>
class Block
{
public:

	Block()
	{
#if defined(GENERIC_ATTRIBS) && defined(CLASSIC_ATTRIBS)
		assert(!(attrib0Enabled && attrib16Enabled));
		assert(!(attrib1Enabled && attrib17Enabled));
		assert(!(attrib2Enabled && attrib18Enabled));
		assert(!(attrib3Enabled && attrib19Enabled));
		assert(!(attrib4Enabled && attrib20Enabled));
		assert(!(attrib5Enabled && attrib21Enabled));
		assert(!(attrib6Enabled && attrib22Enabled));
		assert(!(attrib7Enabled && attrib23Enabled));
		assert(!(attrib8Enabled && attrib24Enabled));
		assert(!(attrib9Enabled && attrib25Enabled));
		assert(!(attrib10Enabled && attrib26Enabled));
		assert(!(attrib11Enabled && attrib27Enabled));
		assert(!(attrib12Enabled && attrib28Enabled));
		assert(!(attrib13Enabled && attrib29Enabled));
		assert(!(attrib14Enabled && attrib30Enabled));
		assert(!(attrib15Enabled && attrib31Enabled));
#endif

#ifdef CLASSIC_ATTRIBS
		// classic attribs 6 and 7 don't map to anything
		assert(!attrib6Enabled);
		assert(!attrib7Enabled);
#endif

		reset();
	}
	~Block()
	{
		reset();
	}
	void reset()
	{
		_mode = prim_none;
		_bufferPos = 0;
		_vertexCount = 0;
	}

	void begin(int mode)
	{
		assert(_mode == prim_none);

		_mode = mode;

		glBindBuffer(GL_ARRAY_BUFFER, 0);

#ifdef CLASSIC_ATTRIBS
		if (attrib0Enabled)
		{
			glVertexPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib0Offset);
			glEnableClientState(GL_VERTEX_ARRAY);
		}
		if (attrib1Enabled)
		{
			glWeightPointerARB(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib1Offset);
			glEnableClientState(GL_WEIGHT_ARRAY_ARB);
		}
		if (attrib2Enabled)
		{
			glNormalPointer(GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib2Offset);
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		if (attrib3Enabled)
		{
			glColorPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib3Offset);
			glEnableClientState(GL_COLOR_ARRAY);
		}
		if (attrib4Enabled)
		{
			glSecondaryColorPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib4Offset);
			glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
		}
		if (attrib5Enabled)
		{
			glFogCoordPointer(GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib5Offset);
			glEnableClientState(GL_FOG_COORD_ARRAY);
		}
		if (attrib6Enabled)
		{

		}
		if (attrib7Enabled)
		{

		}
		if (attrib8Enabled)
		{
			glClientActiveTexture(GL_TEXTURE0);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib8Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib9Enabled)
		{
			glClientActiveTexture(GL_TEXTURE1);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib9Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib10Enabled)
		{
			glClientActiveTexture(GL_TEXTURE2);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib10Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib11Enabled)
		{
			glClientActiveTexture(GL_TEXTURE3);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib11Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib12Enabled)
		{
			glClientActiveTexture(GL_TEXTURE4);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib12Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib13Enabled)
		{
			glClientActiveTexture(GL_TEXTURE5);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib13Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib14Enabled)
		{
			glClientActiveTexture(GL_TEXTURE6);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib14Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib15Enabled)
		{
			glClientActiveTexture(GL_TEXTURE7);
			glTexCoordPointer(4, GL_FLOAT, vertexStride, ((unsigned char*)_buffer) + attrib15Offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
#endif
#ifdef GENERIC_ATTRIBS
		if (attrib16Enabled)
		{
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib16Offset);
			glEnableVertexAttribArray(0);
		}
		if (attrib17Enabled)
		{
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib17Offset);
			glEnableVertexAttribArray(1);
		}
		if (attrib18Enabled)
		{
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib18Offset);
			glEnableVertexAttribArray(2);
		}
		if (attrib19Enabled)
		{
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib19Offset);
			glEnableVertexAttribArray(3);
		}
		if (attrib20Enabled)
		{
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib20Offset);
			glEnableVertexAttribArray(4);
		}
		if (attrib21Enabled)
		{
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib21Offset);
			glEnableVertexAttribArray(5);
		}
		if (attrib22Enabled)
		{
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib22Offset);
			glEnableVertexAttribArray(6);
		}
		if (attrib23Enabled)
		{
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib23Offset);
			glEnableVertexAttribArray(7);
		}
		if (attrib24Enabled)
		{
			glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib24Offset);
			glEnableVertexAttribArray(8);
		}
		if (attrib25Enabled)
		{
			glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib25Offset);
			glEnableVertexAttribArray(9);
		}
		if (attrib26Enabled)
		{
			glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib26Offset);
			glEnableVertexAttribArray(10);
		}
		if (attrib27Enabled)
		{
			glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib27Offset);
			glEnableVertexAttribArray(11);
		}
		if (attrib28Enabled)
		{
			glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib28Offset);
			glEnableVertexAttribArray(12);
		}
		if (attrib29Enabled)
		{
			glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib29Offset);
			glEnableVertexAttribArray(13);
		}
		if (attrib30Enabled)
		{
			glVertexAttribPointer(14, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib30Offset);
			glEnableVertexAttribArray(14);
		}
		if (attrib31Enabled)
		{
			glVertexAttribPointer(15, 4, GL_FLOAT, GL_FALSE, vertexStride, ((unsigned char*)_buffer) + attrib31Offset);
			glEnableVertexAttribArray(15);
		}
#endif
	}

	void end()
	{
		assert(_mode != prim_none);

		flush();

#ifdef CLASSIC_ATTRIBS
		if (attrib0Enabled)
		{
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if (attrib1Enabled)
		{
			glDisableClientState(GL_WEIGHT_ARRAY_ARB);
		}
		if (attrib2Enabled)
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		if (attrib3Enabled)
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}
		if (attrib4Enabled)
		{
			glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
		}
		if (attrib5Enabled)
		{
			glDisableClientState(GL_FOG_COORD_ARRAY);
		}
		if (attrib6Enabled)
		{

		}
		if (attrib7Enabled)
		{

		}
		if (attrib8Enabled)
		{
			glClientActiveTexture(GL_TEXTURE0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib9Enabled)
		{
			glClientActiveTexture(GL_TEXTURE1);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib10Enabled)
		{
			glClientActiveTexture(GL_TEXTURE2);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib11Enabled)
		{
			glClientActiveTexture(GL_TEXTURE3);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib12Enabled)
		{
			glClientActiveTexture(GL_TEXTURE4);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib13Enabled)
		{
			glClientActiveTexture(GL_TEXTURE5);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib14Enabled)
		{
			glClientActiveTexture(GL_TEXTURE6);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (attrib15Enabled)
		{
			glClientActiveTexture(GL_TEXTURE7);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
#endif
#ifdef GENERIC_ATTRIBS
		if (attrib16Enabled)
		{
			glDisableVertexAttribArray(0);
		}
		if (attrib17Enabled)
		{
			glDisableVertexAttribArray(1);
		}
		if (attrib18Enabled)
		{
			glDisableVertexAttribArray(2);
		}
		if (attrib19Enabled)
		{
			glDisableVertexAttribArray(3);
		}
		if (attrib20Enabled)
		{
			glDisableVertexAttribArray(4);
		}
		if (attrib21Enabled)
		{
			glDisableVertexAttribArray(5);
		}
		if (attrib22Enabled)
		{
			glDisableVertexAttribArray(6);
		}
		if (attrib23Enabled)
		{
			glDisableVertexAttribArray(7);
		}
		if (attrib24Enabled)
		{
			glDisableVertexAttribArray(8);
		}
		if (attrib25Enabled)
		{
			glDisableVertexAttribArray(9);
		}
		if (attrib26Enabled)
		{
			glDisableVertexAttribArray(10);
		}
		if (attrib27Enabled)
		{
			glDisableVertexAttribArray(11);
		}
		if (attrib28Enabled)
		{
			glDisableVertexAttribArray(12);
		}
		if (attrib29Enabled)
		{
			glDisableVertexAttribArray(13);
		}
		if (attrib30Enabled)
		{
			glDisableVertexAttribArray(14);
		}
		if (attrib31Enabled)
		{
			glDisableVertexAttribArray(15);
		}
#endif

#ifdef CLASSIC_ATTRIBS
		if (attrib9Enabled || attrib10Enabled || attrib11Enabled || attrib12Enabled || attrib13Enabled || attrib14Enabled || attrib15Enabled)
		{
			// go back to a good state
			glClientActiveTexture(GL_TEXTURE0);
		}
#endif

		reset();
	}

	void flush()
	{
		assert(_mode != prim_none);

		int primCount = 0;
		int primVertexCount = 0;
		GLenum primMode = 0;
		switch (_mode)
		{
		case prim_none:
			break;

		case prim_points:
			primCount = _vertexCount;
			primVertexCount = primCount;
			primMode = GL_POINTS;
			break;

		case prim_line_strip:
			primCount = _vertexCount - 1;
			primVertexCount = primCount + 1;
			primMode = GL_LINE_STRIP;
			break;

		case prim_lines:
			primCount = _vertexCount / 2;
			primVertexCount = primCount * 2;
			primMode = GL_LINES;
			break;

		case prim_triangle_strip:
			primCount = _vertexCount - 2;
			primVertexCount = primCount + 2;
			primMode = GL_TRIANGLE_STRIP;
			break;

		case prim_triangle_fan:
			primCount = _vertexCount - 2;
			primVertexCount = primCount + 2;
			primMode = GL_TRIANGLE_FAN;
			break;

		case prim_triangles:
			primCount = _vertexCount / 3;
			primVertexCount = primCount * 3;
			primMode = GL_TRIANGLES;
			break;

		case prim_quads:
#ifdef EMULATE_QUADS
			primCount = (_vertexCount / 6) * 2;
			primVertexCount = primCount * 3;
			primMode = GL_TRIANGLES;
#else
			primCount = _vertexCount / 4;
			primVertexCount = primCount * 4;
			primMode = GL_QUADS;
#endif
			break;
		}

		if (primCount > 0)
		{
			glDrawArrays(primMode, 0, primVertexCount);

			if (_mode == prim_line_strip)
			{
				// preserve last element of the strip
				primVertexCount -= 1;
			}
			else if (_mode == prim_triangle_strip)
			{
				// preserve last two elements of the strip
				primVertexCount -= 2;
			}
			else if (_mode == prim_triangle_fan)
			{
				// preserve last element of the fan
				primVertexCount -= 1;
			}

			int newVertexCount = _vertexCount - primVertexCount;
			assert(newVertexCount >= 0);

			if (_mode == prim_triangle_fan)
			{
				if (newVertexCount > 0)
				{
					// preserve first element of the fan
					memmove(_buffer + attribCount, _buffer + (_vertexCount - newVertexCount) * attribCount, newVertexCount * vertexStride);
				}
				newVertexCount += 1;
			}
			else
			{
				if (newVertexCount > 0)
				{
					memmove(_buffer, _buffer + (_vertexCount - newVertexCount) * attribCount, newVertexCount * vertexStride);
				}
			}

			_vertexCount = newVertexCount;
			_bufferPos = _vertexCount * attribCount;
		}
	}

	void attrib4f(int index, float x, float y, float z, float w)
	{
		pushAttrib(index, AttribValue(x, y, z, w));
	}
	void attrib3f(int index, float x, float y, float z)
	{
		pushAttrib(index, AttribValue(x, y, z));
	}
	void attrib2f(int index, float x, float y)
	{
		pushAttrib(index, AttribValue(x, y));
	}
	void attrib1f(int index, float x)
	{
		pushAttrib(index, AttribValue(x));
	}

	void attrib4fv(int index, const float *v)
	{
		pushAttrib(index, AttribValue(v[0], v[1], v[2], v[3]));
	}
	void attrib3fv(int index, const float *v)
	{
		pushAttrib(index, AttribValue(v[0], v[1], v[2]));
	}
	void attrib2fv(int index, const float *v)
	{
		pushAttrib(index, AttribValue(v[0], v[1]));
	}

	// other data formats, normalized and unnormalized...

//private:

	void pushAttrib(int index, const AttribValue &value)
	{
		assert(index >= 0 && index < maxAttribs);
		assert(attribEnabled[index]);

		_currentValue[index] = value;

		if (index == 0 || index == 16)
		{
			// provoking vertex

			if (_bufferPos + attribCount > AttribBufferSize)
			{
				// not enough room
				flush();
			}

#ifdef CLASSIC_ATTRIBS
			if (attrib0Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[0];
			}
			if (attrib1Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[1];
			}
			if (attrib2Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[2];
			}
			if (attrib3Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[3];
			}
			if (attrib4Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[4];
			}
			if (attrib5Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[5];
			}
			if (attrib6Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[6];
			}
			if (attrib7Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[7];
			}
			if (attrib8Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[8];
			}
			if (attrib9Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[9];
			}
			if (attrib10Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[10];
			}
			if (attrib11Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[11];
			}
			if (attrib12Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[12];
			}
			if (attrib13Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[13];
			}
			if (attrib14Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[14];
			}
			if (attrib15Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[15];
			}
#endif
#ifdef GENERIC_ATTRIBS
			if (attrib16Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[16];
			}
			if (attrib17Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[17];
			}
			if (attrib18Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[18];
			}
			if (attrib19Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[19];
			}
			if (attrib20Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[20];
			}
			if (attrib21Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[21];
			}
			if (attrib22Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[22];
			}
			if (attrib23Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[23];
			}
			if (attrib24Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[24];
			}
			if (attrib25Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[25];
			}
			if (attrib26Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[26];
			}
			if (attrib27Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[27];
			}
			if (attrib28Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[28];
			}
			if (attrib29Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[29];
			}
			if (attrib30Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[30];
			}
			if (attrib31Enabled)
			{
				_buffer[_bufferPos++] = _currentValue[31];
			}
#endif

			_vertexCount++;

#ifdef EMULATE_QUADS
			if (_mode == prim_quads && _vertexCount % 6 == 4)
			{
				if (_bufferPos + attribCount * 3 > AttribBufferSize)
				{
					// not enough room
					flush();
				}

				unsigned int v0 = _bufferPos - attribCount * 4;
				unsigned int v2 = _bufferPos - attribCount * 2;
				unsigned int v3 = _bufferPos - attribCount;
				for (unsigned int i = 0; i < attribCount; i++)
				{
					// move v4 to the end
					_buffer[_bufferPos + attribCount + i] = _buffer[v3 + i];
					// replicate v0 and v2
					_buffer[_bufferPos - attribCount + i] = _buffer[v0 + i];
					_buffer[_bufferPos + i] = _buffer[v2 + i];
				}

				_bufferPos += attribCount * 2;
				_vertexCount += 2;
			}
#endif
		}
	}

#ifdef CLASSIC_ATTRIBS
	enum {attrib0Enabled = (Attribs & attrib_position) ? 1 : 0};
	enum {attrib1Enabled = (Attribs & attrib_weight) ? 1 : 0};
	enum {attrib2Enabled = (Attribs & attrib_normal) ? 1 : 0};
	enum {attrib3Enabled = (Attribs & attrib_color) ? 1 : 0};
	enum {attrib4Enabled = (Attribs & attrib_color_secondary) ? 1 : 0};
	enum {attrib5Enabled = (Attribs & attrib_fogcoord) ? 1 : 0};
	enum {attrib6Enabled = (Attribs & attrib_classic6) ? 1 : 0};
	enum {attrib7Enabled = (Attribs & attrib_classic7) ? 1 : 0};
	enum {attrib8Enabled = (Attribs & attrib_texcoord0) ? 1 : 0};
	enum {attrib9Enabled = (Attribs & attrib_texcoord1) ? 1 : 0};
	enum {attrib10Enabled = (Attribs & attrib_texcoord2) ? 1 : 0};
	enum {attrib11Enabled = (Attribs & attrib_texcoord3) ? 1 : 0};
	enum {attrib12Enabled = (Attribs & attrib_texcoord4) ? 1 : 0};
	enum {attrib13Enabled = (Attribs & attrib_texcoord5) ? 1 : 0};
	enum {attrib14Enabled = (Attribs & attrib_texcoord6) ? 1 : 0};
	enum {attrib15Enabled = (Attribs & attrib_texcoord7) ? 1 : 0};
#else
	enum {attrib0Enabled = 0};
	enum {attrib1Enabled = 0};
	enum {attrib2Enabled = 0};
	enum {attrib3Enabled = 0};
	enum {attrib4Enabled = 0};
	enum {attrib5Enabled = 0};
	enum {attrib6Enabled = 0};
	enum {attrib7Enabled = 0};
	enum {attrib8Enabled = 0};
	enum {attrib9Enabled = 0};
	enum {attrib10Enabled = 0};
	enum {attrib11Enabled = 0};
	enum {attrib12Enabled = 0};
	enum {attrib13Enabled = 0};
	enum {attrib14Enabled = 0};
	enum {attrib15Enabled = 0};
#endif
#ifdef GENERIC_ATTRIBS
	enum {attrib16Enabled = (Attribs & attrib_0) ? 1 : 0};
	enum {attrib17Enabled = (Attribs & attrib_1) ? 1 : 0};
	enum {attrib18Enabled = (Attribs & attrib_2) ? 1 : 0};
	enum {attrib19Enabled = (Attribs & attrib_3) ? 1 : 0};
	enum {attrib20Enabled = (Attribs & attrib_4) ? 1 : 0};
	enum {attrib21Enabled = (Attribs & attrib_5) ? 1 : 0};
	enum {attrib22Enabled = (Attribs & attrib_6) ? 1 : 0};
	enum {attrib23Enabled = (Attribs & attrib_7) ? 1 : 0};
	enum {attrib24Enabled = (Attribs & attrib_8) ? 1 : 0};
	enum {attrib25Enabled = (Attribs & attrib_9) ? 1 : 0};
	enum {attrib26Enabled = (Attribs & attrib_10) ? 1 : 0};
	enum {attrib27Enabled = (Attribs & attrib_11) ? 1 : 0};
	enum {attrib28Enabled = (Attribs & attrib_12) ? 1 : 0};
	enum {attrib29Enabled = (Attribs & attrib_13) ? 1 : 0};
	enum {attrib30Enabled = (Attribs & attrib_14) ? 1 : 0};
	enum {attrib31Enabled = (Attribs & attrib_15) ? 1 : 0};
#else
	enum {attrib16Enabled = 0};
	enum {attrib17Enabled = 0};
	enum {attrib18Enabled = 0};
	enum {attrib19Enabled = 0};
	enum {attrib20Enabled = 0};
	enum {attrib21Enabled = 0};
	enum {attrib22Enabled = 0};
	enum {attrib23Enabled = 0};
	enum {attrib24Enabled = 0};
	enum {attrib25Enabled = 0};
	enum {attrib26Enabled = 0};
	enum {attrib27Enabled = 0};
	enum {attrib28Enabled = 0};
	enum {attrib29Enabled = 0};
	enum {attrib30Enabled = 0};
	enum {attrib31Enabled = 0};
#endif
	static int attribEnabled[maxAttribs];

#ifdef CLASSIC_ATTRIBS
	enum {attrib0Offset = 0};
	enum {attrib1Offset = attrib0Offset + (attrib0Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib2Offset = attrib1Offset + (attrib1Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib3Offset = attrib2Offset + (attrib2Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib4Offset = attrib3Offset + (attrib3Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib5Offset = attrib4Offset + (attrib4Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib6Offset = attrib5Offset + (attrib5Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib7Offset = attrib6Offset + (attrib6Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib8Offset = attrib7Offset + (attrib7Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib9Offset = attrib8Offset + (attrib8Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib10Offset = attrib9Offset + (attrib9Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib11Offset = attrib10Offset + (attrib10Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib12Offset = attrib11Offset + (attrib11Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib13Offset = attrib12Offset + (attrib12Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib14Offset = attrib13Offset + (attrib13Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib15Offset = attrib14Offset + (attrib14Enabled ? 4 * sizeof(float) : 0)};
#else
	enum {attrib0Offset = 0};
	enum {attrib1Offset = 0};
	enum {attrib2Offset = 0};
	enum {attrib3Offset = 0};
	enum {attrib4Offset = 0};
	enum {attrib5Offset = 0};
	enum {attrib6Offset = 0};
	enum {attrib7Offset = 0};
	enum {attrib8Offset = 0};
	enum {attrib9Offset = 0};
	enum {attrib10Offset = 0};
	enum {attrib11Offset = 0};
	enum {attrib12Offset = 0};
	enum {attrib13Offset = 0};
	enum {attrib14Offset = 0};
	enum {attrib15Offset = 0};
#endif
#ifdef GENERIC_ATTRIBS
	enum {attrib16Offset = attrib15Offset + (attrib15Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib17Offset = attrib16Offset + (attrib16Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib18Offset = attrib17Offset + (attrib17Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib19Offset = attrib18Offset + (attrib18Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib20Offset = attrib19Offset + (attrib19Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib21Offset = attrib20Offset + (attrib20Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib22Offset = attrib21Offset + (attrib21Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib23Offset = attrib22Offset + (attrib22Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib24Offset = attrib23Offset + (attrib23Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib25Offset = attrib24Offset + (attrib24Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib26Offset = attrib25Offset + (attrib25Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib27Offset = attrib26Offset + (attrib26Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib28Offset = attrib27Offset + (attrib27Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib29Offset = attrib28Offset + (attrib28Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib30Offset = attrib29Offset + (attrib29Enabled ? 4 * sizeof(float) : 0)};
	enum {attrib31Offset = attrib30Offset + (attrib30Enabled ? 4 * sizeof(float) : 0)};
#else
	enum {attrib16Offset = 0};
	enum {attrib17Offset = 0};
	enum {attrib18Offset = 0};
	enum {attrib19Offset = 0};
	enum {attrib20Offset = 0};
	enum {attrib21Offset = 0};
	enum {attrib22Offset = 0};
	enum {attrib23Offset = 0};
	enum {attrib24Offset = 0};
	enum {attrib25Offset = 0};
	enum {attrib26Offset = 0};
	enum {attrib27Offset = 0};
	enum {attrib28Offset = 0};
	enum {attrib29Offset = 0};
	enum {attrib30Offset = 0};
	enum {attrib31Offset = 0};
#endif
#ifdef GENERIC_ATTRIBS
	enum {vertexStride = attrib31Offset + (attrib31Enabled ? 4 * sizeof(float) : 0)};
#else
	enum {vertexStride = attrib15Offset + (attrib15Enabled ? 4 * sizeof(float) : 0)};
#endif
	enum {attribCount = vertexStride / (4 * sizeof(float))};
	static int attribOffset[maxAttribs];

	int _mode;
	unsigned int _bufferPos;
	unsigned int _vertexCount;
	AttribValue _currentValue[maxAttribs];
	AttribValue _buffer[AttribBufferSize];
};
template <unsigned int AttribBufferSize, unsigned int Attribs>
int Block<AttribBufferSize, Attribs>::attribEnabled[maxAttribs] = 
{
	attrib0Enabled,
	attrib1Enabled,
	attrib2Enabled,
	attrib3Enabled,
	attrib4Enabled,
	attrib5Enabled,
	attrib6Enabled,
	attrib7Enabled,
	attrib8Enabled,
	attrib9Enabled,
	attrib10Enabled,
	attrib11Enabled,
	attrib12Enabled,
	attrib13Enabled,
	attrib14Enabled,
	attrib15Enabled,
	attrib16Enabled,
	attrib17Enabled,
	attrib18Enabled,
	attrib19Enabled,
	attrib20Enabled,
	attrib21Enabled,
	attrib22Enabled,
	attrib23Enabled,
	attrib24Enabled,
	attrib25Enabled,
	attrib26Enabled,
	attrib27Enabled,
	attrib28Enabled,
	attrib29Enabled,
	attrib30Enabled,
	attrib31Enabled,
};
template <unsigned int AttribBufferSize, unsigned int Attribs>
int Block<AttribBufferSize, Attribs>::attribOffset[maxAttribs] = 
{
	attrib0Offset,
	attrib1Offset,
	attrib2Offset,
	attrib3Offset,
	attrib4Offset,
	attrib5Offset,
	attrib6Offset,
	attrib7Offset,
	attrib8Offset,
	attrib9Offset,
	attrib10Offset,
	attrib11Offset,
	attrib12Offset,
	attrib13Offset,
	attrib14Offset,
	attrib15Offset,
	attrib16Offset,
	attrib17Offset,
	attrib18Offset,
	attrib19Offset,
	attrib20Offset,
	attrib21Offset,
	attrib22Offset,
	attrib23Offset,
	attrib24Offset,
	attrib25Offset,
	attrib26Offset,
	attrib27Offset,
	attrib28Offset,
	attrib29Offset,
	attrib30Offset,
	attrib31Offset,
};

template <int PERMUTATION>
inline void setAttrib(int index, const AttribValue &value)
{
	assert(index >= 0 && index < maxAttribs);

	switch (index)
	{
#ifdef CLASSIC_ATTRIBS
	case index_position:
		assert(0);
		break;

	case index_weight:
		{
			AttribValue v = value;
			glWeightfvARB(4, &v.x);
		}
		break;

	case index_normal:
		glNormal3f(value.x, value.y, value.z);
		break;

	case index_color:
		glColor4f(value.x, value.y, value.z, value.w);
		break;

	case index_color_secondary:
		glSecondaryColor3fv(&value.x);
		break;

	case index_fogcoord:
		glFogCoordfv(&value.x);
		break;

	case index_classic6:
		assert(0);
		break;

	case index_classic7:
		assert(0);
		break;

	case index_texcoord0:
		glMultiTexCoord4f(0, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord1:
		glMultiTexCoord4f(1, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord2:
		glMultiTexCoord4f(2, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord3:
		glMultiTexCoord4f(3, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord4:
		glMultiTexCoord4f(4, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord5:
		glMultiTexCoord4f(5, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord6:
		glMultiTexCoord4f(6, value.x, value.y, value.z, value.w);
		break;

	case index_texcoord7:
		glMultiTexCoord4f(7, value.x, value.y, value.z, value.w);
		break;
#endif

#ifdef GENERIC_ATTRIBS
	case index_0:
		glVertexAttrib4fv(0, &value.x);
		break;

	case index_1:
		glVertexAttrib4fv(1, &value.x);
		break;

	case index_2:
		glVertexAttrib4fv(2, &value.x);
		break;

	case index_3:
		glVertexAttrib4fv(3, &value.x);
		break;

	case index_4:
		glVertexAttrib4fv(4, &value.x);
		break;

	case index_5:
		glVertexAttrib4fv(5, &value.x);
		break;

	case index_6:
		glVertexAttrib4fv(6, &value.x);
		break;

	case index_7:
		glVertexAttrib4fv(7, &value.x);
		break;

	case index_8:
		glVertexAttrib4fv(8, &value.x);
		break;

	case index_9:
		glVertexAttrib4fv(9, &value.x);
		break;

	case index_10:
		glVertexAttrib4fv(10, &value.x);
		break;

	case index_11:
		glVertexAttrib4fv(11, &value.x);
		break;

	case index_12:
		glVertexAttrib4fv(12, &value.x);
		break;

	case index_13:
		glVertexAttrib4fv(13, &value.x);
		break;

	case index_14:
		glVertexAttrib4fv(14, &value.x);
		break;

	case index_15:
		glVertexAttrib4fv(15, &value.x);
		break;
#endif
	}
}

// force compilation so we can mix different GL renderers in the same app
#define Attrib4f(index, x, y, z, w) Attrib4f_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, x, y, z, w)
#define Attrib3f(index, x, y, z) Attrib3f_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, x, y, z)
#define Attrib2f(index, x, y) Attrib2f_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, x, y)
#define Attrib1f(index, x) Attrib1f_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, x)

#define Attrib4fv(index, v) Attrib4fv_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, v)
#define Attrib3fv(index, v) Attrib3fv_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, v)
#define Attrib2fv(index, v) Attrib2fv_template<GLOBAL_SET_ATTRIB_PERMUTATION>(index, v)

template <int forceUnique>
inline void Attrib4f_template(int index, float x, float y, float z, float w)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(x, y, z, w));
}
template <int forceUnique>
inline void Attrib3f_template(int index, float x, float y, float z)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(x, y, z));
}
template <int forceUnique>
inline void Attrib2f_template(int index, float x, float y)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(x, y));
}
template <int forceUnique>
inline void Attrib1f_template(int index, float x)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(x));
}

template <int forceUnique>
inline void Attrib4fv_template(int index, const float *v)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(v[0], v[1], v[2], v[3]));
}
template <int forceUnique>
inline void Attrib3fv_template(int index, const float *v)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(v[0], v[1], v[2]));
}
template <int forceUnique>
inline void Attrib2fv_template(int index, const float *v)
{
	setAttrib<GLOBAL_SET_ATTRIB_PERMUTATION>(index, AttribValue(v[0], v[1]));
}

} // RI

#endif
