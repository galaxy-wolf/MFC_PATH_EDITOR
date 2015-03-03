#ifndef UNIFORM_STD140_H
#define UNIFORM_STD140_H

#include "matrix.h"
#include "align.h"

// see the glsl/gl3.2 spec for rules about std140 layout
namespace std140
{
	ALIGN_START(8) struct vec2
	{
		float x, y;
	} ALIGN_END(8);
	ALIGN_START(16) struct vec3
	{
		float x, y, z;
	} ALIGN_END(16);
	ALIGN_START(16) struct vec4
	{
		float x, y, z, w;
	} ALIGN_END(16);


	ALIGN_START(8) struct ivec2
	{
		int x, y;
	} ALIGN_END(8);
	ALIGN_START(16) struct ivec3
	{
		int x, y, z;
	} ALIGN_END(16);
	ALIGN_START(16) struct ivec4
	{
		int x, y, z, w;
	} ALIGN_END(16);


	ALIGN_START(16) struct floatArrayElem
	{
		float x;
		float pad[3];
	} ALIGN_END(16);
	ALIGN_START(16) struct vec2ArrayElem
	{
		float x, y;
		float pad[2];
	} ALIGN_END(16);
	ALIGN_START(16) struct vec3ArrayElem
	{
		float x, y, z;
		float pad;
	} ALIGN_END(16);
	ALIGN_START(16) struct vec4ArrayElem
	{
		float x, y, z, w;
	} ALIGN_END(16);


	ALIGN_START(16) struct intArrayElem
	{
		int x;
		int pad[3];
	} ALIGN_END(16);
	ALIGN_START(16) struct ivec2ArrayElem
	{
		int x, y;
		int pad[2];
	} ALIGN_END(16);
	ALIGN_START(16) struct ivec3ArrayElem
	{
		int x, y, z;
		int pad;
	} ALIGN_END(16);
	ALIGN_START(16) struct ivec4ArrayElem
	{
		int x, y, z, w;
	} ALIGN_END(16);

	typedef ALIGN_START(16) matrix ALIGN_END(16) mat4;
};

#endif
