#ifndef _TEXTURE_H_
#define _TEXTURE_H_
/***************************************************
To read a jpg image file and download
it as a texture map for openGL
Derived from Tom Lane's example.c
-- Obtain & install jpeg stuff from web
(jpeglib.h, jerror.h jmore.h, jconfig.h,jpeg.lib)
****************************************************/
// windows ��ͷ�ļ�˳�����н����ģ����������Ǽ��ѣ�

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#pragma comment(lib, "../../common/jpeg.lib")
#include "../../common/jpeg.h"
#include "../../common/jpeglib.h"
#include "../../common/jerror.h"
#include <Windows.h>
#include <string.h>

//�����ݱ���Ϊbmpͼ��ĺ���
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
GLuint LoadJPEG(const char* FileName);


//================================
//================================

#endif