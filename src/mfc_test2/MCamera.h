#pragma once
#ifndef _MCAMERA_H
#define _MCAMERA_H
#define _CRT_SECURE_NO_WARNINGS
#include "../../common/tiny_obj_loader.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
// render
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../common/uniform.h"
#include "../../common/shader_glsl.h"
#include "../../common/tiny_jpeg_decoder.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "texture.h"
#define REF_WIDTH  800//1024
#define REF_HEIGHT  600//768
#define PI 3.1415926535898

#include "stdafx.h"
#include "PropertiesWnd.h"
#include "OutputWnd.h"
#include "MVector3.h"
#include <vector>
#include <stack>
#include "3ds.h"
#include "TextureObjects.h"
#define NORMAL_MODE 1
#define SELECT_MODE 2
#define CAPTURE_MODE 3
#define PREVIEW_MODE 4
#define CHANGE_CENTER_MODE 5

/* POINT type*/
#define NORMAL_POINT		'N'
#define STOP_POINT			'S'
#define ZOOM_POINT			'Z'
#define PARALLEL_VIEW_POINT	'P'
#define ROTATE_VIEW_POINT	'R'
#define LINK_POINT			'L'
/* point type */

#define  MAX_MODEL_NUM 1000
using namespace std;
struct ModelParameter{
	char modelPath[200];
	int texture;
	int use_cubemap;
	float diffuse[3];
	int useobj;
	float glossness;
	int changeble ;
};
struct ColorDefine{
	char name[100];
	float diffuse[3];
};
class PointInfo
{
public:
	PointInfo(void);
	PointInfo(const PointInfo &point);
	Vector3 eye;
	Vector3 center;
	Vector3 up;
	// 与前一关键点之间的帧数, 小于1时, 没有意义, 这个点会被忽略
	int cf;
	char PointType;
	// 这一点被LINK_POINT类型点引用的次数， 0 表示没有被引用
	int linkNum;
	// PointType == LINK_POINT 时， 该值为引用点的下标。
	// 例如 该点引用2号位置的点， linkTo = 2
	int linkTo;
	//unsigned int time;
	PointInfo  operator = (const PointInfo & p);
	bool operator== (const PointInfo &p);
};
class StackInfo
{
public:
	int pos;
	vector<PointInfo> bak;
	void clear(){
		bak.clear();
		pos = -1;
	}
};
class GPU_Buffer
{
public:
	std::vector<GLuint> VBOs;
	std::vector<GLuint> NBOs;
	std::vector<GLuint> IBOs;
	std::vector<size_t> Indices_nums;
	std::vector<GLuint> UVBOs;
	std::vector<int> materialIDs;
	std::vector<GLuint> Textures;
	std::vector<glm::vec3> baseColors;
public:
	void clear(){
		VBOs.clear();
		NBOs.clear();
		IBOs.clear();
		Indices_nums.clear();
		UVBOs.clear();
		materialIDs.clear();
		Textures.clear();
		baseColors.clear();

	}
};
class MCamera
{
public:
	MCamera(void);
	~MCamera(void);


public: 
	/* show info */
	void Ui_ShowInfo(string) ;
	void Ui_ClearInfoPanel(void);
	/* show info */

	/* capture button */
	void SetPreview(BOOL pre) {Preview = pre;}
	BOOL GetPreview(void) {return Preview;}
	void SetCapture(BOOL cap) { Capture = cap;}
	BOOL GetCapture(void) {return Capture;}
	void SetPause (BOOL pau) { Pause = pau;}
	BOOL GetPause (void) {return Pause;}
	int GetRunMode (void) { return RunMode;}
	void SetRunMode (int r) ;
	/* capture button */

	/* point */
	void AddPoint(unsigned int  pos, const PointInfo & point);
	void DeletePoint(unsigned int  pos);
	const PointInfo& GetPointDetail(unsigned int pos);
	void ChangePointInfo(unsigned int pos, const PointInfo & point);
	vector<PointInfo>::size_type GetPointsNum(void);
	unsigned int MCamera::GetFrameId(unsigned int pos);
	void addLink(unsigned int pos);
	void deleteLink(unsigned int pos);
	void CopyToClipBoard(const PointInfo & p){ ClipBoard = p;}
	const PointInfo & GetClipBoard(void){ return ClipBoard;}

	void BackUpPointList(unsigned int pos);
	int undo();
	int redo();
	void reloadBackUp(StackInfo & backUp);
	void UpdateUndoStack(unsigned int pos);
	void UpdateUndoStackAndClearRedoStack(unsigned int pos);
	void UpdateRedoStack(unsigned int pos);
	bool CanUndo() { return !undoStack.empty(); }
	bool CanRedo() { return !redoStack.empty(); }
	/* point */

	/* Render */
	void Load_3d_File(const CString & fileName);
	void _Load_3d_File();
	void Draw_3d_Model();
	void Destroy_3d_Model();
	BOOL Load_3DS_Object(int pos, int filePos);
	void Draw_3DS_Object(int pos, int pX, int pY, int pZ, int pSize);
	void Destroy_3DS_Object(int pos);
	void RenderScene(void);
	void UpdateM_eye();
	/* Render */
	/* model */
	BOOL ReadModelDescFile(const CString & fileName);
	BOOL GetColor(char *s, float *color);
	/* model */

	/* keyBorad */
	void keyDown(unsigned int key) { keys[key] = TRUE; }
	void keyUp(unsigned int key) {keys[key] = FALSE; }
	void ModeKeysActions(void );
	
	/* keyBorad */

	/* select point */
	void SelectPoints( GLvoid);
	void DrawPoints();
	void DrawMesh();
	void setCurPoint(unsigned int p) {curPoint = p; }
	// 在当前点的斜上方观察
	void WatchCurPoint(void);
	void MoveToCurPoint(void);
	void LookCurPointFromUp(void);
	unsigned int GetCurPoint(void) {return curPoint;}
	void setMeshY(double y){MeshY = y;}
	void setCanSelectFlag(BOOL f) {canSelectFlag = f;}
	BOOL GetCanSelectFlag(void) { return canSelectFlag;}
	void setSelectPointType(int type) {selectPointType = type;}
	int GetSelectPointType(void ) { return selectPointType;}
	/* select point */

	/* set the camera */
	Vector3 getPosition()   {	return m_eye;		}
	Vector3 getView()	    {	return m_center;			}
	Vector3 getUpVector()   {	return m_up;		}
	double   getSpeed()      {   return m_Speed;         }
	void SetViewHwnd(HWND hwnd) {ViewHwnd = hwnd;}
	void setSpeed(double speed) { m_Speed  = speed; }
	void rotateView(double angle, double X, double Y, double Z);
	void UpdateM_center(); 
	void yawCamera(double speed);
	void moveCamera(double speed);
	void UpCamera(double speed);
	void DownCamera(double speed);
	void setLook();
	/* set the camera */
	

	/* messageBox */
	BOOL GetShowMessageBox(void) {return ShowMessageBox;}
	void SetShowMessageBox(BOOL s){ShowMessageBox = s;}
	/* messageBox */
	/* capture camera */
	BOOL checkPointValid();
	int CaptureCamera(BOOL bShow);
	void CalDelta(BOOL bShow);
	void saveSceneImage(const char * fileName);
	void QuitCapture(void);
	/* capture camera */

	/* edit point view */
	void initChangeCenter(void);
	void UpdatePointList(unsigned int pos, const PointInfo &p);
	void SetMouseMiddle(void);
	void BackUpCurPoint(void){ PointBak = PointList[curPoint];}
	void EditCurPoint(void);
	void AddLookFromUpY(void) {if (LookFromUpY < Max_LookFromUpY)  LookFromUpY += Wid_LookFromUpY;
	m_eye.y = LookFromUpY; MeshRanger = LookFromUpY; MeshWid = MeshRanger / 50;
	}
	void SubLookFromUpY(void) {if (LookFromUpY > Min_LookFromUpY) LookFromUpY -= Wid_LookFromUpY;
	m_eye.y = LookFromUpY; MeshRanger = LookFromUpY; MeshWid = MeshRanger / 50;
	}
	/* edit point view */
	
	/* save point */
	void SavePoint(const CString & fileName);
	void LoadPoint(const CString & fileName);
	void SavePath(const CString & fileName);
	/* save point */
	 
	/* gluPerspective */
	GLdouble GetFovey(void){return fovy;}
	GLdouble GetZNear(void){return zNear;}
	GLdouble GetZFar (void) {return zFar;}
	void SetPerspective(GLdouble m_fovy, GLdouble m_zNear, GLdouble m_zFar)
	{
		bFovyChange = TRUE;
		fovy = m_fovy;
		zNear = m_zNear;
		zFar = m_zFar;
	}
	/* gluPerspective */
	
	/* cx cy */
	void SetCx(int x){ cx = x;}
	void SetCy( int y ) { cy = y;}
	/* cx cy */
	bool LoadObjModel(const char * file);

private: 
	bool createGPUbuffers(std::vector<tinyobj::shape_t> shapes,
		std::vector<tinyobj::material_t> materials);
	void RenderObjModel();
	void CreateArrowBuffer(int accuracy);
	void DrawArrow();
public:
	class CPropertiesWnd * UI_Point;
	class COutputWnd * UI_Info;
private:
	vector<PointInfo> PointList;
	stack< StackInfo > undoStack;
	stack< StackInfo > redoStack;
	StackInfo _backup;

	BOOL Preview;
	BOOL Capture;
	BOOL Pause;
	
	/*load 3ds */
	CLoad3DS g_Load3ds;								// This is 3DS class.  This should go in a good model class.
	t3DModel g_3DModel[MAX_MODEL_NUM];								// This holds the 3D Model info that we load in
	int g_3dModelNum;
	CTextures Textures;
	UINT g_Texture[1000] ;						// This holds the texture info, referenced by an ID
	int   g_ViewMode;
	BOOL LoadingModel;
	float m_frame[6];
	struct ModelParameter modelParameter[MAX_MODEL_NUM];
	int modelParaNum;
	struct ColorDefine colorBoard[100];
	int colorBoardNum;
	/*load 3ds */

	/* keyBorad */
	bool	keys[512];			// Array Used For The Keyboard Routine	键盘数组，可处理按键冲突
	bool	isKeys[512];		//按键被按下且经过了处理, 用于在连续按下的按键时，只进行一次处理的情形；
	
	/* keyBorad */
	
	/* view  camera**/
	Vector3        m_eye;      /**< 位置 */
	Vector3        m_center;          /**< 朝向 */
	Vector3        m_up;      /**< 向上向量 */
	double          m_Speed;         /**< 速度 */
	HWND	ViewHwnd;
	/* view  camera**/

	/* Save Path */
	Vector3 p_eye;
	Vector3 p_center;
	Vector3 p_up;
	/* Save Path */

	/* clipBoard */
	PointInfo ClipBoard;
	/* clipBoard */

	/* messageBox*/
	BOOL ShowMessageBox;
	/* messageBox*/
	/*select point */
	double MeshRanger;
	double MeshWid;
	GLdouble MeshY;
	BOOL canSelectFlag;
	int selectPointType;
	GLdouble LookFromUpY;
	GLdouble Max_LookFromUpY;
	GLdouble Min_LookFromUpY;
	GLdouble Wid_LookFromUpY;
	/*select point */
	/* edit point */
	unsigned int curPoint;
	PointInfo PointBak;
	BOOL FakePreview;
	/* edit point */
	 
	/* capture */
	int RunMode ;
	/* capture */

	/* 模型大小*/
	GLdouble scaleF;
	/* 模型大小*/
	/* capture camera */
	BOOL Captureing;
	unsigned int nextPoint;
	Vector3 EyeDelta;
	Vector3 CenterDelta;
	Vector3 UpDelta;
	unsigned int fps ;
	int cf; //两点之间的帧数
	unsigned int f;
	unsigned int fId;
	/* capture camera */
	/* gluPerspective */
	GLdouble fovy;
	GLdouble zNear;
	GLdouble zFar;
	BOOL bFovyChange;
	int cx;
	int cy;
	/* gluPerspective */

	// GPU buffer 记录：：
	GPU_Buffer gBuffer;
	GLuint arrowVBO;
	GLuint arrowIBO;
	int arrowIndexSize;
	float AABB[10];
	float xzBoard;
	glm::vec3 modelCenter;
	GLSLShaderManager * _shaderManager;

	// 模型矩阵
	glm::mat4 P;
	glm::mat4x4 V;
	glm::mat4x4 M;
	glm::mat4x4 MVP;
	bool GLSLinited;
};


#endif	//_MCAMERA_H