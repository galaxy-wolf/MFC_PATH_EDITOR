#include "StdAfx.h"
#include "MCamera.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include "jpge.h"
using namespace std;
MCamera::MCamera(void):g_ViewMode(GL_TRIANGLES),
m_Speed(0.5f), GLSLinited(false)
{
	memset(g_Texture, NULL, 1000*sizeof(UINT));
	memset(keys, FALSE, 512*sizeof(bool));
	memset(isKeys, FALSE, 512*sizeof(bool));
	memset(AABB, 0, 10*sizeof(float));

	PointInfo p;
	p.PointType = NORMAL_POINT;
	//p.PointType = ROTATE_VIEW_POINT;
	p.cf = 0;
	PointList.push_back(p);


	RunMode = SELECT_MODE;

	MeshRanger = 5000.0;
	MeshWid = 50.0;
	MeshY = 0.0f;

	canSelectFlag = FALSE;
	selectPointType = NORMAL_POINT;

	scaleF = 0.1;
	LookFromUpY = 90;
	Max_LookFromUpY = 99;
	Min_LookFromUpY = 50;
	Wid_LookFromUpY = 5;
	/* capture camera*/
	fps = 30;
	/* capture camera*/
	FakePreview = FALSE;

	bFovyChange = FALSE;
	fovy = 45.0;
	zNear = 0.2f;
	zFar = 50000;
	m_eye = Vector3(0.0, 500.0 * scaleF, 5.0 * scaleF);
	m_center = Vector3(0.0, 0.0, 0.0);
	m_up = Vector3(0.0, 1.0, 0.0);

	g_3dModelNum = 0;
	colorBoardNum = 0;
	modelParaNum = 0;

	LoadingModel = TRUE;

	ShowMessageBox = FALSE;


	
}
MCamera::~MCamera(void)
{
}
void MCamera::Ui_ShowInfo(string info)
{
	UI_Info->ShowInfo(info);
}
void MCamera::Ui_ClearInfoPanel()
{
	UI_Info->ClearInfo();
}
void MCamera::RenderScene(void)
{
	if (!GLSLinited)
	{
		// init glsl 
		if (glewInit()){
			printf("Unable to initialize GLEW ....\n");
			return;
		}
		gBuffer.clear();
		_shaderManager = new GLSLShaderManager("shaders");
		if (!_shaderManager->startup()){
			printf("init: failed to create shaderManager");
			return;
		}
		_shaderManager->loadProgram("shader", 0, 0, 0, "shader");
		CreateArrowBuffer(32);
		GLSLinited = true;
		return;
	}
	if(LoadingModel){
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static PointInfo p;
	static int drawTime=0;
	static vector<PointInfo>::iterator it ;

	//�趨���
	setLook();
	// mode �л�
	ModeKeysActions();

	//<=- add the draw scene code -=>
	/* before draw scene*/
	switch (RunMode)
	{
	case SELECT_MODE:
	case NORMAL_MODE:
	case CHANGE_CENTER_MODE:
		DrawMesh();
		break;
	default:
		break;
	}

	/* draw scene*/
	//����ģ��
	RenderObjModel();

	/* after draw scene*/
	switch (RunMode)
	{
	case SELECT_MODE:
		DrawPoints();
		SelectPoints();
		//�������
		UpdateM_eye();
		Captureing = FALSE;
		break;
	case NORMAL_MODE:
		DrawPoints();
		//�������
		UpdateM_eye();
		UpdateM_center();
		Captureing = FALSE;
		break;

	case CAPTURE_MODE:
		CaptureCamera(TRUE);
		break;
	case PREVIEW_MODE:
		if (FakePreview == FALSE)
			CaptureCamera(TRUE);
		break;
	case CHANGE_CENTER_MODE:
		if (curPoint == 0)
			break;
		EditCurPoint();
		it = PointList.begin() + curPoint;
		it->center = m_center;
		it->eye = m_eye;
		it->up = m_up;
		UI_Point->ShowPointInfo(curPoint);

		break;
	default:
		break;
	}

}
void MCamera::SetRunMode( int r )
{
	RunMode = r;
	switch(RunMode)
	{
	case NORMAL_MODE:
		Ui_ShowInfo("��ǰģʽ����ͨ");
		break;
	case SELECT_MODE:
		Ui_ShowInfo("��ǰģʽ��ѡ��");
		break;
	case PREVIEW_MODE:
		Ui_ShowInfo("��ǰģʽ��Ԥ��");
		break;
	case CHANGE_CENTER_MODE:
		Ui_ShowInfo("��ǰģʽ���༭��");
		break;
	case CAPTURE_MODE:
		Ui_ShowInfo("��ǰģʽ��¼��");
		break;
	}
}
void MCamera::EditCurPoint(void)
{
	if (curPoint < 1)
		return;
	switch(PointList[curPoint].PointType)
	{
	case NORMAL_POINT:// �����ƶ�
		UpdateM_center();
		UpdateM_eye();
		break;
	case ZOOM_POINT: // ֻ��ǰ���ƶ���
		SetMouseMiddle();
		GLdouble len, l; 
		if (keys['W'] || keys[VK_UP] )
		{
			len = 1.0f;
		}else if (keys['S'] || keys[VK_DOWN])
		{
			len = -1.0f;
		}
		else 
			break;
		m_eye += (PointList[curPoint - 1].center - PointList[curPoint - 1].eye).normalize() * len;
		m_center += (PointList[curPoint - 1].center - PointList[curPoint - 1].eye).normalize() * len;
		break;
	case PARALLEL_VIEW_POINT:// ֻ�ܸı���λ��
		SetMouseMiddle();
		UpdateM_eye();
		m_center = PointList[curPoint - 1].center + m_eye - PointList[curPoint - 1].eye;
		break;
	case ROTATE_VIEW_POINT: // ֻ�ܸı��ķ���
		UpdateM_center();
		break;
	default:
		break;
	}

}
void MCamera::initChangeCenter(void)
{
	vector<PointInfo>::iterator it;

	if (curPoint <= 0 || curPoint > PointList.size())
	{
		curPoint = 1;
		UI_Point->m_wndObjectCombo.SetCurSel(curPoint);
		UI_Point->ShowPointInfo(curPoint);
	}
	it = PointList.begin() + curPoint;
	m_eye = it->eye;
	m_center = it->center;
	m_up = it->up;
	SetMouseMiddle();
}
void MCamera::saveSceneImage(const char * fileName)
{
	GLint ViewPort[4];
	glGetIntegerv(GL_VIEWPORT,ViewPort);
	GLsizei ColorChannel = 3;
	GLsizei bufferSize = ViewPort[2]*ViewPort[3]*sizeof(GLubyte)*ColorChannel;
	GLubyte * ImgData = (GLubyte*)malloc(bufferSize);
	GLubyte * pImag_Data = (GLubyte*)malloc(bufferSize);

	/* ��Ҫ��䣬 */
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadPixels(ViewPort[0],ViewPort[1],ViewPort[2],ViewPort[3],GL_RGB,GL_UNSIGNED_BYTE,ImgData);


	/* OpenGL RGB -> JPEG */

	// since glReadPixels read pixel from leftbottom to righttop 
	// and jpeg use pixels from lefttop to rightbottom ;
	for (int i = 0; i < ViewPort[3]; i++)
	{
		memcpy(pImag_Data + i * ViewPort[2] * 3, ImgData + (ViewPort[3] - 1 - i ) * ViewPort[2] * 3,
			ViewPort[2] * 3 * sizeof(GLubyte));
	}
	jpge::params params;
	params.m_quality = 100;//����ѹ��
	params.m_subsampling =  jpge::H2V2;//(subsampling < 0) ? ((actual_comps == 1) ? jpge::Y_ONLY : jpge::H2V2) : static_cast<jpge::subsampling_t>(subsampling);
	params.m_two_pass_flag = FALSE; // optimize_huffman_tables; // FALSE / TRUE

	/* 3 means RGB*/
	if (!jpge::compress_image_to_jpeg_file(fileName, ViewPort[2], ViewPort[3], 3, pImag_Data, params))
	{
		printf("can't save image %s\n", fileName);
	}

	free(ImgData);
	free(pImag_Data);
}
void MCamera::CalDelta( BOOL bShow)
{
	vector<PointInfo>::iterator it, itNext;
	static char ch[100];
	nextPoint ++;
	sprintf (ch, "��һ�����ǣ� %d\n", nextPoint);
	if (bShow == TRUE)
		Ui_ShowInfo(ch);
	it = PointList.begin() + nextPoint - 1;
	itNext = PointList.begin() + nextPoint;
	/* ��Ϊ ��ǰ*/
	if (bShow == TRUE)
	{
		m_eye = it->eye;
		m_up = it->up;
		m_center = it->center;
	}
	else{
		p_eye = it->eye;
		p_up = it->up;
		p_center = it->center;
	}

	if (nextPoint > PointList.size() - 1) //û����һ������
	{
		cf = -1;// ָʾ
		if (bShow)
			Ui_ShowInfo("û����һ���ؼ����ˣ�");
		return;
	}

	//use frame
	cf = itNext->cf;
	CenterDelta = (itNext->center - it->center) / cf;
	EyeDelta = (itNext->eye - it->eye) / cf;
	UpDelta = (itNext->up - it->up) / cf;
	f = 1;
}
void MCamera::QuitCapture()
{
	// move to up
	/*m_eye = Vector3(0.0, 1000, 50);
	m_center = Vector3(0, 0, 0);*/
	LookCurPointFromUp();
	RunMode = SELECT_MODE;
	Ui_ShowInfo("��ǰģʽ�� ѡ��");
	Pause = Capture = Preview = FALSE;
}
int MCamera::CaptureCamera(BOOL bShow)
{
	static char info[100];
	static char file[100];
	static char *root = "./Capture/";
	// �����̫��
	if (PointList.size() <3)
	{
		if (bShow == TRUE)
			Ui_ShowInfo("���ܽ��н���������ԭ�򣺹ؼ������");
		QuitCapture();
		return -1;
	}
	vector<PointInfo>::iterator it;
	if (Captureing == FALSE) //�״������� ��ʼ��
	{
		if (checkPointValid())
		{
			if (bShow == TRUE)
				Ui_ShowInfo("���ܽ���Ԥ����¼�ƣ�ԭ�򣺹ؼ������");
			QuitCapture();
			return -1;
		}
		//  ��������ͼƬ���ļ���
		if (Capture == TRUE)
		{
			CreateDirectoryA((LPSTR)root, NULL);
		}
		Captureing = TRUE;
		// �ӵ�ǰ�㿪ʼ¼�ơ�
		if (FakePreview == true)
			nextPoint = 1;
		else
		{
			if (curPoint != 0)
				nextPoint = curPoint;
			else
				nextPoint = 1;
		}
		CalDelta(bShow);
		fId = 0;
	}
	else{

		if (Pause == TRUE)
		{
			// do nothing 
		}
		else{
			if (Capture == TRUE)
			{
				// save Images
				//sprintf(file,"%s%d.bmp", root, fId);
				sprintf(file,"%s%d.jpeg", root, fId);
				sprintf(info, "save image %s", file);
				Ui_ShowInfo(info);
				saveSceneImage(file);
			}
			if (cf == -1)// capture success!!
			{
				if (bShow == TRUE)
					Ui_ShowInfo("¼�Ƴɹ������ڳ����ļ����µ�Capture�в鿴¼�ƽ��!!\n");
				QuitCapture();

				return 0;
			}
			//ajust the camera;
			if (f == cf) //�� cf ֡ΪnextPoint
			{
				CalDelta(bShow);
				fId ++;
			}
			else{
				if (bShow == TRUE)
				{
					m_center += CenterDelta;
					m_eye += EyeDelta;
					m_up += UpDelta;
				}
				else
				{
					p_center += CenterDelta;
					p_eye += EyeDelta;
					p_up += UpDelta;
				}
				f ++;
				fId ++;
			}

		}

	}
	return 1;
}
BOOL MCamera::checkPointValid()
{
	char s[100];
	vector<PointInfo>::iterator it = PointList.begin();
	if (PointList.size() < 3)
	{
		Ui_ShowInfo("���󣺹ؼ������");
		return TRUE;
	}
	int i;
	for (i = 2; i < PointList.size(); i ++)
	{
		if ((it + i)->cf < 1)
		{
			sprintf(s, "frame between Point %d and %d is too short", i-1, i);
			Ui_ShowInfo(s);
			return TRUE;
		}
		if ((it + i)->PointType == LINK_POINT && (it + i)->linkTo == 0)
		{
			sprintf(s, "point %d link to invalid point!!",i);
			Ui_ShowInfo(s);
			return TRUE;
		}
	}
	return FALSE;
}
void MCamera::UpdateM_eye()
{
	/** ���̰�����Ӧ */
	if(keys[VK_SHIFT])                        /**< ����SHIFT��ʱ���� */
	{
		setSpeed(xzBoard/1000);
	}
	if(!keys[VK_SHIFT])
	{
		setSpeed(xzBoard/100);
	}
	if(keys[VK_UP] || keys['W'])   /**< ���Ϸ������'W'������ */
		moveCamera(getSpeed());          /**< �ƶ������ */

	if(keys[VK_DOWN] ||keys['S']) /**< ���·������'S'������ */
		moveCamera(-getSpeed());         /**< �ƶ������ */

	if(keys[VK_LEFT] ||keys['A']) /**< ���������'A'������ */
		yawCamera(-getSpeed());          /**< �ƶ������ */

	if(keys[VK_RIGHT] ||keys['D']) /**< ���ҷ������'D'������ */
		yawCamera(getSpeed());            /**< �ƶ������ */
	// select Mode �в��������ƶ�
	if (RunMode != SELECT_MODE)
	{
		if(keys['Q'])// || keys[VK_PRIOR])
			UpCamera(getSpeed());
		if(keys['Z'])// || keys[VK_NEXT])
			DownCamera(getSpeed());
	}
	// �ƶ�mesh
	if (keys[VK_PRIOR])
	{
		MeshY += getSpeed();
		UI_Point->ShowMeshY(MeshY);
	}

	if (keys[VK_NEXT])
	{
		MeshY -= getSpeed();
		UI_Point->ShowMeshY(MeshY);
	}
}
void MCamera::DrawMesh()
{
	GLfloat y = MeshY;
	double i;
	glUniform4fv(_shaderManager->program[0].getUniformLocation("baseColor"), 1,
		&glm::vec4(0.7, 0.7, 0.7, 0.0)[0]);
	glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 0);
	M = glm::mat4x4();
	M = glm::translate(M, glm::vec3(modelCenter.x, 0.0, modelCenter.z));
	MVP = P*V*M;
	glm::mat4x4 MV = V*M;
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("gWorld"),
		1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("MV"),
		1, GL_FALSE, &MV[0][0]);
	glBegin(GL_LINES);
	for (i = 0; i < MeshRanger; i += MeshWid)
	{
		if (i == MeshWid)
			continue;
		glVertex3d( i, y, -1 * MeshRanger);
		glVertex3d(i, y, MeshRanger);
		glVertex3d(-1 * i, y, -1 * MeshRanger);
		glVertex3d(-1 * i, y, MeshRanger);
		glVertex3d(-1 * MeshRanger, y, i);
		glVertex3d(MeshRanger, y, i);
		glVertex3d(-1 * MeshRanger, y, -1 * i);
		glVertex3d(MeshRanger, y, -1 * i);
	}
	
	glEnd();
	
}
void MCamera::CreateArrowBuffer(int accuracy)
{
	std::vector<GLfloat> points;
	std::vector<GLuint> index;
	// up
	points.push_back(0.0);
	points.push_back(15.0);
	points.push_back(0.0);
	for (int i = 0; i < accuracy; i++)
	{
		points.push_back(3.0 *sin(2*PI*(double)i/accuracy));
		points.push_back(10.0);
		points.push_back(3.0 *cos(2*PI*(double)i/accuracy));

		if (i > 0)
		{
			index.push_back(0);
			index.push_back(i);
			index.push_back(i + 1);
		}
	}
	index.push_back(0);
	index.push_back(accuracy);
	index.push_back(1);

	int base = points.size() / 3;
	for (int i = 0; i < accuracy; i++)
	{
		points.push_back(1.0 *sin(2*PI*(double)i/accuracy));
		points.push_back(12.0);
		points.push_back(1.0 *cos(2*PI*(double)i/accuracy));
		points.push_back(1.0 *sin(2*PI*(double)i/accuracy));
		points.push_back(0.0);
		points.push_back(1.0 *cos(2*PI*(double)i/accuracy));
		
		if (i > 0)
		{
			index.push_back(base + (i - 1) * 2);
			index.push_back(base + (i - 1) * 2 + 1);
			index.push_back(base + i * 2+1);

			index.push_back(base + (i - 1) * 2);
			index.push_back(base + i * 2 + 0);
			index.push_back(base + i * 2+1);
			
		}
	}
	index.push_back(base + (accuracy-1) * 2);
	index.push_back(base + (accuracy -1) * 2 + 1);
	index.push_back(base + 1);

	index.push_back(base + (accuracy - 1) * 2);
	index.push_back(base);
	index.push_back(base + 1);

	arrowIndexSize = index.size();
	glGenBuffers(1, &arrowVBO);
	glGenBuffers(1, &arrowIBO);
	glBindBuffer(GL_ARRAY_BUFFER, arrowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(),
		&(points[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrowIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * index.size(),
		&(index[0]), GL_STATIC_DRAW);
}

void MCamera::DrawPoints()
{
	static Vector3 A, B, S, R;
	double k;
	double arrowReSize;
	static int blink = 0;

	// ͸����ͷ
	vector<PointInfo>::iterator it;
	int p;
	
	for (  it = PointList.begin(), p = 0; 
		it != PointList.end(); it++, p++)
	{
		// Ϊ����˸��������curPoint�ص��ĵ�
		if (curPoint != 0 && *it == *(PointList.begin() + curPoint) && p != curPoint)
			continue;
		// ����
		M = glm::mat4x4();
		M = glm::translate(M, glm::vec3(it->eye.x, it->eye.y, it->eye.z));

		R = m_eye - it->eye;
		arrowReSize = R.length() / 20 / 15;
		/*if (arrowReSize < 1.000001)
			arrowReSize = 1;*/
		A = it->center - it->eye;
		k = A.length();
		B = A * (15 * arrowReSize / k);
		S = (B + Vector3(0, 15 * arrowReSize, 0)) / 2;
		/* ����ͷ*/
		glRotated(180.0, S.x, S.y, S.z); // ����ת��center - eye ƽ�е�λ��
		M = glm::rotate(M, (float)180.0, glm::vec3(S.x, S.y, S.z));
		M = glm::scale(M, glm::vec3(arrowReSize, arrowReSize, arrowReSize));

		//green
		glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 0);
		glUniform4fv(_shaderManager->program[0].getUniformLocation("baseColor"), 1,
				&(glm::vec4(0.0, 1.0, 0.0, 0.5)[0]));
		if (curPoint == p && p != 0 )
		{
			blink = (blink++)%25;
			if (blink > 12)
			{
				// blue
				glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 0);
				glUniform4fv(_shaderManager->program[0].getUniformLocation("baseColor"), 1,
					&(glm::vec4(0.0, 0.0, 1.0, 0.5)[0]));
			}
		}
		MVP = P*V*M;
		glm::mat4x4 MV = V*M;
		glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("gWorld"),
			1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("MV"),
			1, GL_FALSE, &MV[0][0]);
		DrawArrow();
	}
	
	M = glm::mat4x4();
	MVP = P*V*M;
	glm::mat4x4 MV = V*M;
	glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 0);
	glUniform4fv(_shaderManager->program[0].getUniformLocation("baseColor"), 1,
		&glm::vec4(0.0, 1.0, 1.0, 1.0)[0]);
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("gWorld"),
		1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("MV"),
		1, GL_FALSE, &MV[0][0]);
	glBegin(GL_LINE_STRIP);
	for ( vector<PointInfo>::iterator it = PointList.begin() + 1; 
		it != PointList.end(); it++)
	{
		glVertex3d(it->eye.x, it->eye.y, it->eye.z);
	}
	glEnd();
	

}
void MCamera::WatchCurPoint(void)
{
	// curPoint == 0 ok
	vector<PointInfo>::iterator it = PointList.begin() + curPoint;
	m_center = it->eye;
	m_eye = it->eye + Vector3(100 * scaleF, 50 * scaleF, 100 * scaleF);
	//
	m_eye = Vector3(modelCenter.x, LookFromUpY,	modelCenter.z + 0.1);
	m_center = Vector3(modelCenter.x, modelCenter.y, modelCenter.z);
	m_up = Vector3(0.0, 1.0, 0.0);
}
void MCamera::MoveToCurPoint(void)
{
	if (curPoint == 0)
		return;
	m_eye = PointList[curPoint].eye;
	m_up = PointList[curPoint].up;
	m_center = PointList[curPoint].center;
}
void MCamera::LookCurPointFromUp( void )
{
	// PointList[0] .eye == (0, 0, 0)  PointList[0] .center== (0, 0, 0);
	m_center = PointList[curPoint].eye;
	m_eye = Vector3(m_center.x, LookFromUpY, m_center.z + LookFromUpY / 20);
	m_up = PointList[curPoint].up;
	m_eye = Vector3(modelCenter.x, LookFromUpY, modelCenter.z + 0.1);
	m_center = Vector3(modelCenter.x, modelCenter.y, modelCenter.z);
	m_up = Vector3(0.0, 1.0, 0.0);
}
void MCamera::SelectPoints( )
{
	// ��UI����ѡ����ӵ�����ͺ� ���ܿ�ʼѡ�㡣
	if (canSelectFlag == FALSE)
	{
		return;
	}
	//MeshY = m_eye.y - 50.0f;
	POINT mousePos;
	glm::ivec4 viewport;
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;  /*  OpenGL y coordinate position  */
	GLdouble x1, y1, z1;  /*  returned world x, y, z coords  */
	GLdouble x0, y0, z0;
	GLdouble x2, y2, z2;
	Vector3 m_temp;
	PointInfo p;
	vector<PointInfo>::iterator it;
	GLdouble len;


	if (keys[VK_LBUTTON] && !isKeys[VK_LBUTTON]) //�������
	{


		isKeys[VK_LBUTTON] = TRUE;
		//��� ���ߵ�һ���� V1��
		GetCursorPos (&mousePos);
		ScreenToClient(ViewHwnd, &mousePos);
		glGetIntegerv (GL_VIEWPORT, &viewport[0]);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
		/*  note viewport[3] is height of window in pixels  */
		realy = viewport[3] - mousePos.y - 1;
		glm::vec3 p1 = glm::unProject(
			glm::vec3(mousePos.x, realy, 0.0),
			V,
			P,
			viewport);
		x1 = p1.x;
		y1 = p1.y;
		z1 = p1.z;
		

		//����һ������ ��������ӵ�V0
		x0 = m_eye.x;
		y0 = m_eye.y;
		z0 = m_eye.z;

		//������֪�� Y ������� ѡ��� V2
		if ( (y1 - y0 > 0 && y1 - y0 > 0.00001) || 
			(y0 - y1 > 0 && y0 - y1 > 0.00001)) //��ֹ�� 0 ����
		{
			y2 = MeshY;
			x2 = (x1 - x0) * (y2 - y0) / (y1 - y0) + x0;
			z2 = (z1 - z0) * (y2 - y0) / (y1 - y0) + z0;

			//�� ��V0 - V1) . (V0 - V2) > 0 �ж� V3 ����������
			if ( (x0 - x1) * (x0 - x2) + (y0 -y1) * (y0 -y2) 
				+ (z0 - z1) * (z0 -z2) > 0 )
			{
				p.eye.x = x2;
				p.eye.y = y2;
				p.eye.z = z2;
				canSelectFlag = FALSE;
				// NOMAL_POINT ZOOM_POINT PARALLEL_POINT
				// ����Դ�
				p.PointType = selectPointType;
				switch(selectPointType)
				{
				case NORMAL_POINT:
					p.center = Vector3(modelCenter.x, modelCenter.y, modelCenter.z);
					break;
				case PARALLEL_VIEW_POINT:
					if (curPoint == 0)
						return;
					it = PointList.begin() + curPoint;
					p.center = it->center + p.eye - it->eye;
					break;
				case ZOOM_POINT:
					if (curPoint == 0)
					{
						return;
					}
					it = PointList.begin() + curPoint;
					p.eye.y = it->eye.y;			// p ͶӰ��it->eye.y ƽ�� ��С��� 
					// ���� ѡ������һ���ĳ���
					len = (p.eye - it->eye).length();
					if ( (it->center - it->eye).dotProduct(p.eye - it->eye) < 0 )
						len *= -1;
					// �� ��ǰ���eye center �� center - eye ƽ�� len ���ȼ�Ϊ p ��eye center
					p.eye = it->eye + (it->center - it->eye).normalize() * len;
					p.center = it->center + (it->center - it->eye).normalize() * len;
					break;

				default:
					Ui_ShowInfo("can not select Point with mouse in\
								this add point mode\n");
					return;
					break;
				}
				p.linkTo = 0;
				p.linkNum = 0;
				// �����ͨ��������������ӵ�
				AddPoint(curPoint + 1, p);
				/* �ڽ����и��� curpoint */
				UI_Point->UpdatePropUiAfterAddPoint(curPoint + 1);
			}

		}

	}
	if ( !keys[VK_LBUTTON]) //����ͷ�
	{
		isKeys[VK_LBUTTON] = FALSE;
	}
}
void MCamera::SetMouseMiddle(void)
{
	CRect rect;
	GetClientRect(ViewHwnd, &rect);
	POINT p;
	p.x = rect.Width()/2;
	p.y = rect.Height()/2;
	ClientToScreen(ViewHwnd, &p);
	SetCursorPos(p.x, p.y);
}
void MCamera::ModeKeysActions(void )
{
	if (keys['E'] && !isKeys['E'])
	{
		isKeys['E'] = TRUE;
		if (RunMode == NORMAL_MODE)
		{
			RunMode = SELECT_MODE;
			Ui_ShowInfo("��ǰģʽ: �༭\n");
			LookCurPointFromUp();
		}
	}
	if (!keys['E'])
	{
		isKeys['E'] = FALSE;
	}

	if (keys['R'] && !isKeys['R'])
	{
		isKeys['R'] = TRUE;
		if (RunMode == SELECT_MODE)
		{
			RunMode = NORMAL_MODE;
			Ui_ShowInfo("��ǰģʽ����ͨ\n");
			WatchCurPoint();
		}
	}
	if (!keys['R'])
	{
		isKeys['R'] = FALSE;
	}

	if (keys[VK_ESCAPE] && !isKeys[VK_ESCAPE])
	{
		isKeys[VK_ESCAPE] = TRUE;
		
		if (RunMode == CHANGE_CENTER_MODE)
		{
			PointInfo p;
			RunMode = SELECT_MODE;
			Ui_ShowInfo("��ǰģʽ���༭��\n");
			p = PointList[curPoint];
			PointList[curPoint] = PointBak;
			ChangePointInfo(curPoint, p);
			LookCurPointFromUp();
		}
		// set cursor to middle
		SetMouseMiddle();	
	}
	if (!keys[VK_ESCAPE])
		isKeys[VK_ESCAPE] = FALSE;
	// Enter ��ʾ������Ĳ��˳�
	if (keys[VK_RETURN] & !isKeys[VK_RETURN])
	{
		if (RunMode == CHANGE_CENTER_MODE)
		{
			PointInfo p;
			RunMode = SELECT_MODE;
			Ui_ShowInfo("RunMode: Select Mode\n");
			p = PointList[curPoint];
			PointList[curPoint] = PointBak;
			// move to look down position
			LookCurPointFromUp();
		}
		// set cursor to middle
		SetMouseMiddle();
	}
	if (!keys[VK_RETURN])
		isKeys[VK_RETURN] = FALSE;
}
// ���� obj �ļ��� Ĭ��mtl �ļ���obj��ͬһ�ļ���
bool MCamera::LoadObjModel(const char * file)
{
	int k = 0;
	for (int i = 0; file[i] != '\0'; i++)
	{
		if (file[i] == '\\')
			k = i;
	}
	char path[1024];
	strcpy(path, file);
	path[++k] = '\0';

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	memset(AABB, 0, 10 * sizeof(float));
	std::string err = tinyobj::LoadObj(shapes, materials, file, path, AABB);
	if (!err.empty()) {
		Ui_ShowInfo("����ģ���ļ�����");
		Ui_ShowInfo(err);
		return false;
	}

	// ����ǰһ��ģ��
	gBuffer.clear();
	//PrintInfo(shapes, materials);
	Ui_ShowInfo("��ģ�����뵽GPU��......");
	createGPUbuffers(shapes, materials);

	// �������ģ��AABB ȷ����������Ĳ���
	xzBoard = std::fmax(std::abs(AABB[0] - AABB[0 + 3]),
		std::abs(AABB[2] - AABB[2 + 3]));
	MeshRanger = xzBoard * 2;
	MeshWid = MeshRanger / 50;
	MeshY = std::fmin(AABB[1], AABB[1 + 3]);
	/*if (MeshWid < 0.5)
		MeshWid = 0.5;
	if (MeshWid > 500)
		MeshWid = 500;*/
	modelCenter.x = (AABB[0] + AABB[0 + 3]) / 2;
	modelCenter.y = (AABB[1] + AABB[1 + 3]) / 2;
	modelCenter.z = (AABB[2] + AABB[2 + 3]) / 2;
	m_eye = Vector3(modelCenter.x, 2* std::fmax(xzBoard, std::fmax(AABB[1], AABB[1+3])),
		modelCenter.z + 0.1);
	m_center = Vector3(modelCenter.x, modelCenter.y, modelCenter.z);
	LookFromUpY = 2 * std::fmax(xzBoard, std::fmax(AABB[1], AABB[1 + 3]));
	Max_LookFromUpY = 20 * std::fmax(xzBoard, std::fmax(AABB[1], AABB[1 + 3]));
	Min_LookFromUpY = std::fmax(xzBoard, std::fmax(AABB[1], AABB[1 + 3]));
	Wid_LookFromUpY = std::fmax(xzBoard, std::fmax(AABB[1], AABB[1 + 3])) / 5;
	printf("%f, %f, %f\n", AABB[0], AABB[1], AABB[2]);
	printf("%f, %f, %f\n", AABB[3], AABB[4], AABB[5]);
	return true;
}
bool
MCamera::createGPUbuffers(
std::vector<tinyobj::shape_t> shapes,
std::vector<tinyobj::material_t> materials)
{
	GLuint VBO;
	GLuint NBO;
	GLuint IBO;
	size_t Indices_num;
	int materialID;
	GLuint UVBO;
	glm::vec3 baseColor;

	for (size_t i = 0; i < shapes.size(); i++) {
		
		VBO = NBO = IBO = UVBO = -1;
		// һ��mesh �����ж��material ������
		if (shapes[i].mesh.material_ids.size() > 1)
		{
			materialID = shapes[i].mesh.material_ids[0];
		}
		else{
			materialID = -1;
		}
		if (shapes[i].mesh.positions.size() > 0)
		{
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.positions.size(),
				&(shapes[i].mesh.positions[0]), GL_STATIC_DRAW);
		}
		
		if (shapes[i].mesh.normals.size() > 0)
		{
			glGenBuffers(1, &NBO);
			glBindBuffer(GL_ARRAY_BUFFER, NBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.normals.size(),
				&(shapes[i].mesh.normals[0]), GL_STATIC_DRAW);
		}

		if (shapes[i].mesh.indices.size() > 0){
			glGenBuffers(1, &IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shapes[i].mesh.indices.size(),
				&(shapes[i].mesh.indices[0]), GL_STATIC_DRAW);
		}
		Indices_num = shapes[i].mesh.indices.size();
		if (shapes[i].mesh.texcoords.size() > 0)
		{
			glGenBuffers(1, &UVBO);
			glBindBuffer(GL_ARRAY_BUFFER, UVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*shapes[i].mesh.texcoords.size(),
				&(shapes[i].mesh.texcoords[0]), GL_STATIC_DRAW);
		}
		gBuffer.VBOs.push_back(VBO);
		gBuffer.NBOs.push_back(NBO);
		gBuffer.IBOs.push_back(IBO);
		gBuffer.Indices_nums.push_back(Indices_num);
		gBuffer.UVBOs.push_back(UVBO);
		gBuffer.materialIDs.push_back(materialID);
	}

	for (size_t i = 0; i < materials.size(); i++){
		// get file name 
		baseColor = glm::vec3((float)1.0, (float)1.0, (float)1.0);
		gBuffer.baseColors.push_back(glm::vec3(materials[i].ambient[0],
			materials[i].ambient[1], materials[i].ambient[2]));
		std::string flag = "\\";
		std::string::size_type position;
		position = materials[i].ambient_texname.rfind(flag);
		std::string fileName;
		if (position != materials[i].ambient_texname.npos)
		{
			fileName = materials[i].ambient_texname.substr(position + 1);
		}
		else{
			fileName.empty();
		}
		if (fileName.size() == 0)
		{
			
			gBuffer.Textures.push_back(-1);
			continue;
		}
		gBuffer.Textures.push_back(LoadJPEG(materials[i].ambient_texname.c_str()));

	}

	// create textures

	std::cout << "create GPU buffer ok" << std::endl;
	Ui_ShowInfo("��ģ������GPU�ɹ�");
	return true;
}
void MCamera::RenderObjModel()
{
	static float angle = 0;
	angle += 0.1;
	
	glUseProgram(_shaderManager->program[0].handle);
	M = glm::mat4x4();
	MVP = P*V*M;
	glm::mat4x4 MV = V*M;
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("gWorld"),
		1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("MV"),
		1, GL_FALSE, &MV[0][0]);
	glUniform1i(_shaderManager->program[0].getUniformLocation("myTextureSampler"), 0);
	for (size_t i = 0; i < gBuffer.VBOs.size(); i++) {
		if (gBuffer.materialIDs[i] == -1 || gBuffer.Textures[gBuffer.materialIDs[i]] == -1)// no texture
		{// base color; useTexture = false;
			glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 0);
			glUniform4fv(_shaderManager->program[0].getUniformLocation("baseColor"), 1,
				&glm::vec4(gBuffer.baseColors[gBuffer.materialIDs[i]], 0.0)[0]);
		}
		else{
			// useTexture = true;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.Textures[gBuffer.materialIDs[i]]);
			glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 1);
		}
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, gBuffer.VBOs[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if (gBuffer.UVBOs[i] != -1)
		{
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, gBuffer.UVBOs[i]);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
				);

		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gBuffer.IBOs[i]);
		glDrawElements(GL_TRIANGLES, gBuffer.Indices_nums[i], GL_UNSIGNED_INT, 0);
		// �������
		glDisableVertexAttribArray(0);
		if (gBuffer.UVBOs[i] != -1)
		{
			glDisableVertexAttribArray(1);
		}
	}
	//DrawArrow();
	//DrawMesh();
}

void MCamera::DrawArrow()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, arrowVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, gBuffer.UVBOs[0]);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
		);*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrowIBO);
	glDrawElements(GL_TRIANGLES, arrowIndexSize, GL_UNSIGNED_INT, 0);
	// �������
	glDisableVertexAttribArray(0);

/*	glDisableVertexAttribArray(1)*/;
}
// <+- ����ģ�ͣ� ����ģ�� +->
void MCamera::Load_3d_File(const CString & fileName)
{
	LoadingModel = true;
	CStringA strA(fileName);
	char * file = strA.GetBuffer();
	int len;
	char s[5];
	len = strlen(file);
	strncpy(s, file + len - 3, 4);
	if ((s[0] == 'o' || s[0] == 'O') && (s[1] == 'b' || s[1] == 'B') && (s[2] == 'j' || s[2] == 'J')){
		Ui_ShowInfo("��������ģ�ͣ�����ģ�ʹ�С������ʱ�䲻ͬ��һ����Ҫ2min");
		if (false == LoadObjModel(file))
		{
			LoadingModel = false;
			return;
		}
	}
	else
	{
		Ui_ShowInfo("Ŀǰֻ֧��OBJ��ʽ��ģ���ļ���");
		LoadingModel = false;
		return;
	}
	Ui_ShowInfo("ģ������ɹ���");
	LoadingModel = false;
}
void MCamera::Draw_3d_Model()
{
	GLfloat light_position1[] = { -1000000.0, 100000.0, 1000000.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);

	GLfloat light_ambient [] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse [] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 0.1, 0.1, 0.1, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient );
	glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse );
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glShadeModel (GL_SMOOTH);
	/*glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);*/


	GLdouble aspect_ratio;
	// pers
	if (bFovyChange == TRUE)
	{
		UI_Point->ShowPerspective();
		bFovyChange = FALSE;
		glViewport(0, 0, cx, cy);
		aspect_ratio = (GLdouble)cx / (GLdouble)cy;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(GetFovey(), aspect_ratio, GetZNear(), GetZFar());
		glMatrixMode(GL_MODELVIEW);
	}
	for (int i = 0; i < g_3dModelNum; i ++)
		Draw_3DS_Object(i, 0, 0, 0, 1);

}
BOOL MCamera::ReadModelDescFile( const CString & fileName )
{
	FILE *fp= _tfopen(fileName, _T("r"));
	if (fp == NULL)
	{
		Ui_ShowInfo("error: model describe file does not exist!!");
		return FALSE;
	}
	int line = 0;
	int i;
	char s[200];
	char *c;
	while(!feof(fp))
	{
		// �����ļ�������ɫ����
		fgets(s, 200, fp);
		// ɾ����β�ո� Tab �� �س�
		for (i = strlen(s) - 1; i > 0 && (s[i] == '\r' || s[i] == '\n'  
			|| s[i] == ' ' || s[i] == 9) ; i --)
			s[i] = '\0';
		line ++;
		if (feof(fp))
		{
			Ui_ShowInfo("file bad end");
			return FALSE;
		}
		// ^# ��ʾ ��ɫ����
		if (s[0] == '#')
		{
			c = s + 1;
			// space and tab
			while (*c !=' ' && *c != 9 && *c != '\0') c++;
			while ((*c ==' ' || *c == 9)) c++;
			strcpy (colorBoard[colorBoardNum].name, c);
			// check if current exist;
			if (TRUE == GetColor(colorBoard[colorBoardNum].name, colorBoard[colorBoardNum].diffuse))
			{
				sprintf(s, "line %d error %s is defined before\n", line, 
					colorBoard[colorBoardNum].name);
				Ui_ShowInfo(s);
				return FALSE;
			}
			fgets(s, 200, fp);

			for (i = strlen(s) - 1; i > 0 && (s[i] == '\r' || s[i] == '\n'  
				|| s[i] == ' ' || s[i] == 9) ; i --)
				s[i] = '\0';
			line ++;
			if (FALSE == GetColor(s, colorBoard[colorBoardNum].diffuse))
			{
				sprintf(s, "line %d error color\n", line);
				Ui_ShowInfo(s);
				return FALSE;
			}
			colorBoardNum ++;
		}
		else
		{
			strcpy (modelParameter[modelParaNum].modelPath, s);
			fgets(s, 200, fp);

			for (i = strlen(s) - 1; i > 0 && (s[i] == '\r' || s[i] == '\n'  
				|| s[i] == ' ' || s[i] == 9) ; i --)
				s[i] = '\0';
			line ++;
			if (FALSE == GetColor(s, modelParameter[modelParaNum].diffuse))
			{
				sprintf(s, "line %d error color\n", line);
				Ui_ShowInfo(s);
				return FALSE;
			}
			modelParaNum ++;
		}
	}
	return TRUE;
}
float GetFloat(char * s)
{
	char *c;
	float a, b, ab;
	for ( c = s; *c != '\0' && *c != '/'; c++);
	if (*c == '\0')
		sscanf(s,"%f", &ab);
	else
	{
		sscanf(s, "%f/%f", &a, &b);
		ab = a/b;
	}
	return ab;
}
BOOL MCamera::GetColor( char *s, float *color )
{
	int i;
	char *c;
	// define ��ɫ
	if (s[0] > 'a' && s[0] < 'z' || s[0] > 'A' && s[0] < 'Z')
	{
		for ( i = 0; i < colorBoardNum; i ++ )
		{
			if (0== strcmp(s, colorBoard[i].name))
			{
				color[0] = colorBoard[i].diffuse[0];
				color[1] = colorBoard[i].diffuse[1];
				color[2] = colorBoard[i].diffuse[2];
				return TRUE;
			}
		}
		return FALSE;
	}
	else{ // ��ֵ�������ɫ
		char *i1;
		char *i2;
		c = s;
		while(*c != ',' && *c != '\0') c++;
		i1 = c;
		c++;
		while(*c != ',' && *c != '\0') c++;
		i2 = c;
		if ( *i1 == '\0' || *i2 == '\0')
			return FALSE;
		*i1 = '\0';
		i1 ++;
		*i2 = '\0';
		i2 ++;
		color[0] = GetFloat(s);
		color[1] = GetFloat(i1);
		color[2] = GetFloat(i2);
	}
}
// <+- ���´���Ϊ camera ���� ��ʵ�� +->
void MCamera::rotateView(double angle, double x, double y, double z)
{
	Vector3 newView;

	/** ���㷽������ */
	Vector3 view = m_center - m_eye;		

	/** ���� sin ��cosֵ */
	double cosTheta = (double)cos(angle);
	double sinTheta = (double)sin(angle);

	/** ������ת������xֵ */
	newView.x  = (cosTheta + (1 - cosTheta) * x * x)		* view.x;
	newView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* view.y;
	newView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* view.z;

	/** ������ת������yֵ */
	newView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* view.x;
	newView.y += (cosTheta + (1 - cosTheta) * y * y)		* view.y;
	newView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* view.z;

	/** ������ת������zֵ */
	newView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* view.x;
	newView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* view.y;
	newView.z += (cosTheta + (1 - cosTheta) * z * z)		* view.z;

	/** ����������ķ��� */
	m_center = m_eye + newView;
}
void MCamera::UpdateM_center()  // ��κ������ӽǳ�������������
{
	POINT mousePos;	/**< ���浱ǰ���λ�� */
	BOOL flag = TRUE ;
	double angleX = 0.0f;							  /**< �����������ת�Ƕ� */
	double angleY = 0.0f;		                      /**< �����������ת�Ƕ� */					
	static double currentRotY = 0.0f;
	CRect rect;
	GetClientRect(ViewHwnd, &rect);
	POINT p, p1;
	p.x = rect.Width()/2;
	p.y = rect.Height()/2;

	ClientToScreen(ViewHwnd, &p);
	GetCursorPos(&p1);
	p1.x = p.x -p1.x;
	p1.y = p.y -p1.y;

	SetCursorPos(p.x, p.y);

	angleX = (double)( p1.x ) / 1000.0f;		//�����1000���������ȣ���ֵԽ��������Խ��
	angleY = (double)( p1.y ) / 1000.0f;		
	static double lastRotY = 0.0f;      /**< ���ھ�̬������ת�Ƕ� */
	lastRotY = currentRotY; 

	/** ���������������ת�Ƕ� */
	currentRotY += angleY;

	/** ���������ת���ȴ���1.0,���ǽ�ȡ��1.0����ת */
	//if(currentRotY > 1.0f)     
	//{
	//	currentRotY = 1.0f;
	//	
	//	/** ���ݱ���ĽǶ���ת���� */
	//	if(lastRotY != 1.0f) 
	//	{
	//		/** ͨ������ҵ�����ת����ֱ������ */
	//		Vector3 vAxis = m_center - m_eye;
	//		vAxis = vAxis.crossProduct(m_up);
	//		vAxis = vAxis.normalize();
	//		
	//		///��ת
	//		rotateView( 1.0f - lastRotY, vAxis.x, vAxis.y, vAxis.z);
	//	}
	//}
	///** �����ת����С��-1.0,��Ҳ��ȡ��-1.0����ת */
	//else if(currentRotY < -1.0f)
	//{
	//	currentRotY = -1.0f;
	//			
	//	if(lastRotY != -1.0f)
	//	{
	//		
	//		/** ͨ������ҵ�����ת����ֱ������ */
	//		Vector3 vAxis = m_center - m_eye;
	//		vAxis = vAxis.crossProduct(m_up);
	//		vAxis = vAxis.normalize();
	//		
	//		///��ת
	//		rotateView( -1.0f - lastRotY, vAxis.x, vAxis.y, vAxis.z);
	//	}
	//}
	///** �������תangleY�� */
	//else 
	//{	
	//	/** �ҵ�����ת����ֱ���� */
	//	Vector3 vAxis = m_center - m_eye;
	//	vAxis = vAxis.crossProduct(m_up);
	//	vAxis = vAxis.normalize();
	//
	//	///��ת
	//		rotateView(angleY, vAxis.x, vAxis.y, vAxis.z);
	//	}
	Vector3 vAxis = m_center - m_eye;
	vAxis = vAxis.crossProduct(m_up);
	vAxis = vAxis.normalize();
	rotateView(angleY, vAxis.x, vAxis.y, vAxis.z);
	/** ����������ת����� */
	//rotateView(angleX, vAxis.x, vAxis.y, vAxis.z);
	//rotateView(angleX, 0, 1, 0);
	m_up.normalize();
	rotateView(angleX, m_up.x, m_up.y, m_up.z);
}
void MCamera::yawCamera(double speed)
{
	Vector3 yaw;
	Vector3 oldPos,oldView;
	Vector3 cross = m_center - m_eye;
	oldPos = m_eye;
	oldView = m_center;
	cross = cross.crossProduct(m_up);

	///��һ������
	cross.y = 0.0f; //��Ҫy�����
	yaw = cross.normalize();

	m_eye.x += yaw.x * speed;
	m_eye.z += yaw.z * speed;
	m_eye.y += yaw.y * speed;

	m_center.x += yaw.x * speed;
	m_center.z += yaw.z * speed;
	m_center.y += yaw.y * speed;

	/** ���б߽�����޶� */
	/*if(m_center.x > MAP_WIDTH - 20)
	{
	m_eye.x = oldPos.x ;
	m_center.x = oldView.x;
	}

	if(m_center.z > MAP_WIDTH - 20 )
	{
	m_eye.z = oldPos.z ;
	m_center.z = oldView.z;
	}

	if(m_center.y > MAP_WIDTH - 20)
	{
	m_eye.y = oldPos.y ;
	m_center.y = oldView.y;
	}

	*/
}
void MCamera::moveCamera(double speed)
{
	/** ���㷽������ */
	CString str1000;
	Vector3 vector = m_center - m_eye;
	vector.y = 0.0f; //��Ҫy����ģ�
	vector = vector.normalize();         /**< ��λ�� */
	Vector3 oldPos,oldView;
	oldPos = m_eye;
	oldView = m_center;

	/** ��������� */
	m_eye.x += vector.x * speed;    /**< �����ٶȸ���λ�� */
	m_eye.z += vector.z * speed;	
	m_eye.y += vector.y * speed;


	m_center.x += vector.x * speed;		 /**< �����ٶȸ��·��� */	
	m_center.y += vector.y * speed;
	m_center.z += vector.z * speed;

	//   /** ���б߽�����޶� */	
	//if(m_center.x > MAP_WIDTH - 20)
	//{
	//	
	//	m_eye.x = oldPos.x ;
	//	m_center.x = oldView.x;
	//									// ����MFC�е��ַ�������
	////	str1000.Format("%f",m_center.x);    // ����Format����������ת�����ַ�������һ���������Ǳ�������
	////	MessageBox(NULL, str1000, "��ʾ", MB_OK);
	//	
	//}

	//if(m_center.z > MAP_WIDTH - 20)
	//{
	//	m_eye.z = oldPos.z ;
	//	m_center.z = oldView.z;
	//}

	//if(m_center.y > MAP_WIDTH - 20)
	//{
	//	m_eye.y = oldPos.y ;
	//	m_center.y = oldView.y;
	//}

}
void MCamera::UpCamera(double speed)
{
	m_eye.y += speed;
	m_center.y += speed;
}
void MCamera::DownCamera(double speed)
{
	m_eye.y -= speed;
	m_center.y -= speed;
}
void MCamera::setLook()
{

	/** �����ӿ� */
	gluLookAt(m_eye.x, m_eye.y, m_eye.z,	
		m_center.x,	 m_center.y,     m_center.z,	
		m_up.x, m_up.y, m_up.z);
	glm::vec3 center = glm::vec3(m_center.x, m_center.y, m_center.z);
	glm::vec3 up = glm::vec3(m_up.x, m_up.y, m_up.z);
	glm::vec3 eye = glm::vec3(m_eye.x, m_eye.y, m_eye.z);
	V = glm::lookAt(eye, center, up);
	P = glm::perspective((GLfloat)fovy, (GLfloat)cx / (GLfloat)cy, (GLfloat)zNear, (GLfloat)zFar);
	glViewport(0, 0, cx, cy);
	P = glm::perspective((GLfloat)fovy, (GLfloat)cx / (GLfloat)cy, (GLfloat)zNear, (GLfloat)zFar);
	glViewport(0, 0, cx, cy);


}



// <+- ���´���Ϊ ���غͱ���ؼ���+->

void MCamera::SavePoint(const CString  &fileName )
{
	FILE *fp= _tfopen(fileName, _T("w"));
	if (fp == NULL)
	{
		Ui_ShowInfo("����: ���ܴ򿪹ؼ����ļ�!!");
		return;
	}
	vector<PointInfo>::iterator it ;
	fprintf(fp, "Para\n");
	fprintf(fp, "%.6f, %.6f, %.6f\n", fovy, zNear, zFar);
	for (it = PointList.begin()+ 1; it != PointList.end(); it++)
	{
		fprintf(fp,"Point\n");
		fprintf(fp, "%.6f, %.6f, %.6f\n", it->eye.x, it->eye.y, it->eye.z);
		fprintf(fp, "%.6f, %.6f, %.6f\n", it->center.x, it->center.y, it->center.z);
		fprintf(fp, "%.6f, %.6f, %.6f\n", it->up.x, it->up.y, it->up.z);
		fprintf(fp, "%d\n", it->cf);
		fprintf(fp, "%c\n", it->PointType);
		fprintf(fp, "%d\n", it->linkTo);
		fprintf(fp, "%d\n", it->linkNum);
	}
	Ui_ShowInfo("����ؼ���ɹ�!\n");
	fclose(fp);
}
// �ݴ�������
void MCamera::LoadPoint( const CString & fileName )
{
	FILE *fp= _tfopen(fileName, _T("r"));
	if (fp == NULL)
	{
		Ui_ShowInfo("���󣺹ؼ����ļ����ܴ�!!");
		return;
	}

	PointInfo p;
	PointList.clear();
	stack< StackInfo > empty1;
	std::swap(undoStack, empty1);
	stack< StackInfo > empty2;
	std::swap(redoStack, empty2);

	PointList.push_back(p);
	char s[100];
	fgets(s, 100, fp);
	fscanf(fp, "%lf, %lf, %lf\n", &fovy, &zNear, &zFar);
	UI_Point->ShowPerspective();
	bFovyChange = TRUE;
	while (!feof(fp))
	{
		fgets(s, 100, fp); //!= EOF && strcmp(s, "Point\n") != 0);
		fscanf(fp, "%lf, %lf, %lf\n", &p.eye.x, &p.eye.y, &p.eye.z);
		fscanf(fp, "%lf, %lf, %lf\n", &p.center.x, &p.center.y, &p.center.z);
		fscanf(fp, "%lf, %lf, %lf\n", &p.up.x, &p.up.y, &p.up.z);
		fscanf(fp, "%d\n", &p.cf);
		fscanf(fp, "%c\n", &p.PointType);
		fscanf(fp, "%d\n", &p.linkTo);
		fscanf(fp, "%d\n", &p.linkNum);
		PointList.push_back(p);
	}
	if (PointList.size() > 1)
		UI_Point->UpdatePropUiAfterAddPoint(1);
	fclose(fp);
}
void MCamera::SavePath( const CString & fileName )
{
	FILE *fp= _tfopen(fileName, _T("w"));
	if (fp == NULL)
	{
		Ui_ShowInfo("���󣺲��ܴ򿪱����·���ļ�!!");
		return;
	}
	Pause = Capture = FALSE;
	RunMode = PREVIEW_MODE;
	Preview = TRUE;
	FakePreview = TRUE;
	int state = 1;
	state = CaptureCamera(FALSE);
	fprintf(fp, "Para\n");
	fprintf(fp, "%.6f, %.6f, %.6f\n", fovy, zNear, zFar);
	while (state == 1)
	{
		fprintf(fp,"Point\n");
		fprintf(fp, "%.6f, %.6f, %.6f\n", p_eye.x, p_eye.y, p_eye.z);
		fprintf(fp, "%.6f, %.6f, %.6f\n", p_center.x, p_center.y, p_center.z);
		fprintf(fp, "%.6f, %.6f, %.6f\n", p_up.x, p_up.y, p_up.z);
		state = CaptureCamera(FALSE);
	}
	if ( state == 0)
		Ui_ShowInfo("·������ɹ�!\n");
	else
		Ui_ShowInfo("·������ʧ�ܣ�\n");
	FakePreview = FALSE;
	fclose(fp);
}
