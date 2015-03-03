#pragma once
#ifndef _MODEL_H
#define _MODEL_H
//#include "stdafx.h"
#define  TEAPOT 0.51
#define FACKSHADOW
//const int textureNum=15;
//GLuint modelTexture[textureNum];
//char *texturePath[textureNum] = {
//	"./model/floor.jpg",  //0
//	"./model/wood.jpg",    //1
//	"./model/redstone.jpg",  //2
//	"./model/book2.jpg",	//3
//	"./model/book.jpg",     //4
//	"./model/japen2.jpg",   //5
//	"./wp/up.jpg",    //6
//	"./aoBathroom/masic.jpg",     //7
//	"./aoBathroom/walls.jpg",   //8
//	"./aoBathroom/floor.jpg",      //9
//	"./aoBathroom/masic.jpg",         //10 west
//	"./aoBathroom/littleWall.jpg",      //11  esat
//	"./bathroom/sceneSight.jpg" ,  //12   north
//	"./bathroom/2.jpg",       //13  south
//	"./bathroom/masac.jpg",      //9
//};
//const int aoTextureNum =8;
//GLuint randomTex;
//GLuint aoModelTexture[aoTextureNum];
//char * aoTexturePath[aoTextureNum] = 
//{
//	"./shadowMap/shitBathroom.jpg",         //0
//	"./shadowMap/glossyV.bmp",         //1
//	"./shadowMap/hasShiftTooth.jpg",         //2
//
//	"./shadowMap/shiftTooth.jpg",         //03
//	"./shadowMap/hairBrushS.jpg",         //4
//	"./shadowMap/newBath.jpg",         //4
//	"./shadowMap/sink.jpg",         //4
//	"./shadowMap/newSink.jpg",         //4
//};
const int modelNum=50;//57;
//t3DModel model[modelNum];
//t3DModel  changedModel[2];
//t3DModel aoModel[modelNum];
//GLuint modelList[modelNum];
#define CHINAREFLECT 0.185f
#define METAL {0.74901,0.678,0.435}
#define METALREFLECT 0
struct ModelParameter{
	char *modelPath;
	int texture;
	int use_cubemap;
	float diffuse[3];
	int useobj;
	float glossness;
	int changeble ;
};



const struct ModelParameter
modelParameter[]=//modelNum]=
{

	//	{"./bathroom/bowlBody.3ds",      -1,0,{1.0,1.0,1.0},   {0.9,0.9,0.9},   1,  0},

	{"./bathroom/q1.3ds",            -1,0,{192.0/255,129.0/255,74.0/255},    1, 0.0,1},//0.15  //0
	{"./bathroom/q2.3ds",            -1,0,{1.0,1.0,1.0},    1, 0.0,0},//0.15  //1


	{"./aoBathroom/littleBarrier.3ds",      -1,0,{169.0/255,170.0/255,156.0/255},      1,  0,0},  //2
	{"./aoBathroom/littleWall.3ds",       11,0,{0.2,0.3,0.4},     0,  0,0}, //3
	//{"./bathroom/shit2.3ds",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  CHINAREFLECT},

	{"./bathroom/toilet1.3ds",       -1,0,{0.9,0.9,0.9},      0,  0,0},  //4
	{"./bathroom/toilet2.3ds",       -1,0,{0.9,0.9,0.9},      0,  CHINAREFLECT,0},//5
	{"./bathroom/toilet3.3ds",       -1,0,{0.9,0.9,0.9},      0,  0,0},//6
	{"./bathroom/toilet4.3ds",       -1,0,{0.9,0.9,0.9},      0,  CHINAREFLECT,0},//7
	{"./bathroom/toilet5.3ds",       -1,0,{0.9,0.9,0.9},      0,  0,0},//8
	{"./bathroom/toilet6.3ds",       -1,0,{0.9,0.9,0.9},      0,  CHINAREFLECT,0},//9

	//{"./bathroom/fakePlane.3ds",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},

	//{"./bathroom/t3.3DS",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},
	//{"./bathroom/t4.3ds",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},
	//{"./bathroom/t5.3DS",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},
	//{"./bathroom/t6.3ds",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},
	//{"./bathroom/t1.3DS",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},
	//{"./bathroom/twhole.3DS",       -1,0,{0.9,0.9,0.9},   {0.9,0.9,0.9},   0,  0},
	{"./bathroom/washbasin42.3ds",     -1,0,{0.9,0.9,0.9},  0,CHINAREFLECT,0},//15

	{"./bathroom/wash_basin0_t.3ds",       -1,0,{0.9,0.9,0.9},      1,  CHINAREFLECT,0},//10

	{"./bathroom/washbasin1.3DS",       -1,0,{0.9,0.9,0.9},      0,  CHINAREFLECT,0},//11
	{"./bathroom/washbasin2_n.3ds",       -1,0,{0.9,0.9,0.9},     0,  CHINAREFLECT,0},//12
	{"./bathroom/washbasin3_n.3ds",       -1,0,{0.91,0.9,0.9},      0,  CHINAREFLECT,0},//13
	{"./bathroom/washbasin41.3ds",     -1,0,{0.9,0.9,0.9},      0,CHINAREFLECT,0},//14

	{"./bathroom/washbasinCircle.3DS",       -1,0,{0.9,0.9,0.9},      0, 0 ,0},//16
	{"./bathroom/toiletGap1.3DS",      -1,0,{0.9,0.9,0.9},      0,  CHINAREFLECT,0},//17
	{"./bathroom/toiletGap2.3DS",      -1,0,{0.9,0.9,0.9},     0,  0,0},//18
	{"./bathroom/toiletUp1.3DS",      -1,0,{0.9,0.9,0.9},    0,  0,0},//19
	{"./bathroom/toiletUpbody1.3DS",      -1,0,{0.9,0.9,0.9},      0,  CHINAREFLECT,0},//20
	{"./bathroom/toiletUpUp3.3DS",      -1,1,{0.9,0.9,0.9},      0,  0,0},
	{"./bathroom/toiletUpPlane3.3DS",      -1,0,{0.91,0.9,0.9},     0,  CHINAREFLECT,0},
	{"./bathroom/ceil.3ds",            -1,0,{0.9,0.9,0.9},     0,  0,0},//0.15
	//{"./bathroom/ceilMasac.3ds",            13,0,{0.9,0.9,0.9},      0,  0,0},//0.15

	//{"./bathroom/wall_west.3ds",            -1,0,{0.85,0.8,0.85},   {0.3,0.3,0.3},   0,  0},//0.15
	{"./aoBathroom/walls.3ds",            8,0,{0.85,0.8,0.85},     0,  0,0},//0.15
	{"./bathroom/senie.3ds",            12,0,{0.85,0.8,0.85},      0,  0,0},//0.15
	//{"./bathroom/wall_south.3ds",            -1,0,{0.85,0.8,0.85},   {0.3,0.3,0.3},   0,  0},//0.15
	//	{"./bathroom/wall_east.3ds",            -1,0,{0.85,0.8,0.85},   {0.3,0.3,0.3},   0,  0},//0.15
	{"./bathroom/floor.3ds",            -1,0,{0.9,0.9,0.9},     0,  0.00,0},//0.15
	{"./aoBathroom/masic.3ds",            7,0,{1,0,0},      0,  0.1,0},//0.15
	{"./bathroom/paperRoll.3ds",            -1,0,{1,225.0/255,250.0/255},      0,  0,0},//0.15
	{"./bathroom/paperSupport.3ds",            -1,0,{0.7,0.9,0.6},     0,  0,0},//0.15
	//	{"./bathroom/brush.3ds",            -1,0,{0.9,0.9,0.9},   {0.3,0.3,0.3},   0,  0},//0.15
	{"./bathroom/cockTop1.3ds",            -1,0,METAL,     0, METALREFLECT,0},//0.15
	{"./bathroom/cockBody1.3ds",            -1,0,METAL,      0, METALREFLECT,0},//0.15
	{"./bathroom/cockTop2.3ds",            -1,0,METAL,      0, METALREFLECT,0},//0.15
	{"./bathroom/cockBody2.3ds",            -1,0,METAL,      0, METALREFLECT,0},//0.15

	//{"./bathroom/pipe1.3ds",            -1,0,METAL,   {0.3,0.3,0.3},   0, METALREFLECT},//0.15
	//{"./bathroom/pipe21.3ds",            -1,0,METAL,   {0.3,0.3,0.3},   0, METALREFLECT},//0.15
	//{"./bathroom/pipe22.3ds",            -1,0,METAL,   {0.3,0.3,0.3},   0, METALREFLECT},//0.15
	{"./bathroom/wheel1.3ds",            -1,0,METAL,      0, METALREFLECT,0},//0.15
	{"./bathroom/wheel2.3ds",            -1,0,METAL,      0, METALREFLECT,0},//0.15
	{"./bathroom/soapSurport2.3ds",            -1,0,METAL,     1, METALREFLECT,0},//0.15
	{"./bathroom/dish1.3ds",            -1,0,{247.0/255,242.0/255,216.0/255},      0, 0.0,0},//0.15
	{"./bathroom/dish2.3ds",            -1,0,{247.0/255,242.0/255,216.0/255},      0, 0.2,0},//0.15
	{"./bathroom/cup.3ds",            -1,0,{0.9,0.9,0.9},    1, 0},//0.15
	{"./bathroom/toothBrush.3ds",            -1,0,{108.0/255,65.0/255,42.0/255},     0, 0,0},//0.15
	{"./bathroom/toothBrush2.3ds",            -1,0,{88.0/255,163.0/255,22.0/255},      0, 0,0},//0.15
	{"./bathroom/white2.3ds",            -1,0,{1,1,1},      1, 0,0},//0.15
	//{"./bathroom/soap2.3ds",            -1,0,{237.0/255,136.0/255,54.0/255},   {0.3,0.3,0.3},   0, 0},//0.15
	{"./bathroom/toothOnDesk/toothBrush.3ds",            -1,0,{192.0/255,129.0/255,74.0/255},     1, 0,0},//0.15
	{"./bathroom/toothOnDesk/Brush3.3ds",            -1,0,{1,1,1},      1, 0,0},//0.15

	{"./bathroom/newWash.3ds",            -1,0,{0.9,0.9,0.9},     1, 0.0,0},//0.15

	{"./bathroom/mirrorFrame.3ds",            -1,0,{1,219.0/255,207.0/255},     0, 0,0},//0.15
	{"./bathroom/mirror.3ds",            -1,0,{1,219.0/255,207.0/255},     0, 1,0},//0.15
	{"./bathroom/sopportM.3ds",            -1,0,{198.0/255,219.0/255,207.0/255},   1, 0.2,0},//0.15
	{"./bathroom/supportNoM.3ds",            -1,0,{198.0/255,219.0/255,207.0/255},   1, 0.0,0},//0.15

};	


#endif