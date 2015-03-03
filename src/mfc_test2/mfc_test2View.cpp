
// mfc_test2View.cpp : Cmfc_test2View ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "mfc_test2.h"
#endif

#include "mfc_test2Doc.h"
#include "mfc_test2View.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MCamera.h"
extern MCamera		  m_camera;
// Cmfc_test2View

IMPLEMENT_DYNCREATE(Cmfc_test2View, CView)

BEGIN_MESSAGE_MAP(Cmfc_test2View, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cmfc_test2View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// Cmfc_test2View ����/����

Cmfc_test2View::Cmfc_test2View()
{
	// TODO: �ڴ˴���ӹ������
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

Cmfc_test2View::~Cmfc_test2View()
{
}

BOOL Cmfc_test2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.style |= WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

// Cmfc_test2View ����

void Cmfc_test2View::OnDraw(CDC* /*pDC*/)
{
	Cmfc_test2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// Cmfc_test2View ��ӡ


void Cmfc_test2View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cmfc_test2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void Cmfc_test2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void Cmfc_test2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void Cmfc_test2View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cmfc_test2View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Cmfc_test2View ���

#ifdef _DEBUG
void Cmfc_test2View::AssertValid() const
{
	CView::AssertValid();
}

void Cmfc_test2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cmfc_test2Doc* Cmfc_test2View::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmfc_test2Doc)));
	return (Cmfc_test2Doc*)m_pDocument;
}
#endif //_DEBUG


// Cmfc_test2View ��Ϣ�������
BOOL Cmfc_test2View::SetupPixelFormat(void)
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // pfd�ṹ�Ĵ�С 
		1,                              // �汾�� 
		PFD_DRAW_TO_WINDOW |            // ֧���ڴ����л�ͼ 
		PFD_SUPPORT_OPENGL |            // ֧�� OpenGL 
		PFD_DOUBLEBUFFER,               // ˫����ģʽ 
		PFD_TYPE_RGBA,                  // RGBA ��ɫģʽ 
		32,                             // 32 λ��ɫ��� 
		0, 0, 0, 0, 0, 0,               // ������ɫλ 
		0,                              // û�з�͸���Ȼ��� 
		0,                              // ������λλ 
		0,                              // ���ۼƻ��� 
		0, 0, 0, 0,                     // �����ۼ�λ 
		16,                             // 32 λ��Ȼ���     
		0,                              // ��ģ�建�� 
		0,                              // �޸������� 
		PFD_MAIN_PLANE,                 // ���� 
		0,                              // ���� 
		0, 0, 0                         // ���Բ�,�ɼ��Ժ������ģ 

	};
	int pixelFormat;
	// Ϊ�豸������õ���ƥ������ظ�ʽ 
	if((pixelFormat = ChoosePixelFormat(/*m_pDC->GetSafeHdc()*/ hDC, &pfd)) == 0)
	{
		MessageBox( _T("ChoosePixelFormat failed") );
		return FALSE;
	}
	// ������ƥ������ظ�ʽΪ��ǰ�����ظ�ʽ 
	if(SetPixelFormat(/*m_pDC->GetSafeHdc()*/ hDC, pixelFormat, &pfd) == FALSE)
	{
		MessageBox( _T("SetPixelFormat failed") );
		return FALSE;
	}
	return TRUE;
}
BOOL Cmfc_test2View::InitializeOpenGL(void)
{
	//Get a DC for the Client Area
	HWND hWnd = this->GetSafeHwnd();
	m_camera.SetViewHwnd(hWnd);
	hDC = ::GetDC(hWnd);
	
	// ���õ�ǰ�Ļ�ͼ���ظ�ʽ
	if(!SetupPixelFormat())
	{
		return FALSE;
	}

	m_hRC=wglCreateContext(hDC);//m_pDC->GetSafeHdc());
	if(m_hRC == NULL)
	{
		MessageBox(_T("Error Creating RC"));
		return FALSE;
	}
	// ʹ��ͼ������Ϊ��ǰ�����̵ֳĵ�ǰ��ͼ������
	if( wglMakeCurrent(/*m_pDC->GetSafeHdc()*/ hDC, m_hRC) == FALSE)
	{
		MessageBox(_T("Error making RC Current"));
		return FALSE;
	}

	// ������ͼ������
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	//// <-+ load models +->
	////m_camera.Load_3DS_Object();
	//m_camera.Load_3d_File();
	return TRUE;
}


int Cmfc_test2View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	// 
	if (!InitializeOpenGL())
		return -1;
	SetTimer(1, 33, NULL);
	return 0;
}


void Cmfc_test2View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	static int count=0;
	GLdouble aspect_ratio; 
	 printf("%d : %d,  %d\n",count++, cx, cy);
	// TODO: �ڴ˴������Ϣ����������
	if (cy == 0)
		cy = 1;
	m_camera.SetCx(cx);
	m_camera.SetCy(cy);
	glViewport(0, 0, cx, cy);
	aspect_ratio = (GLdouble)cx / (GLdouble)cy;
	
}


BOOL Cmfc_test2View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
}


void Cmfc_test2View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	if (wglGetCurrentContext()!= NULL)
	{
		if(::wglMakeCurrent (NULL, NULL) == FALSE)
		{
			MessageBox(_T("Could not make RC non-current"));
		}
	}
	if(::wglDeleteContext(m_hRC)==FALSE)
	{
		MessageBox(_T("Could not delete RC"));
	}

}

//�������ͼ��ʱ�� �Զ�������ָ����ͼ
void Cmfc_test2View::keyFocus(void)
{
	// Ч�� ������ ÿ��ˢ�½��涼����
	static CRect viewRect;
	static POINT leftBottom, rightTop, mouse;
	GetClientRect(&viewRect);
	GetCursorPos(&mouse);
	leftBottom.y = viewRect.bottom;
	leftBottom.x = viewRect.left;
	rightTop.y = viewRect.top;
	rightTop.x = viewRect.right;
	ClientToScreen(&leftBottom);
	ClientToScreen(&rightTop);
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	int properDock = pMain->m_wndProperties.HitTest(mouse);
	int infoDock = pMain->m_wndOutput.HitTest(mouse);
	if ( m_camera.GetShowMessageBox() == FALSE && 
		(mouse.x > leftBottom.x && mouse.x < rightTop.x)
		&& (mouse.y < leftBottom.y && mouse.y > rightTop.y)
		&& properDock == HTNOWHERE	//���� prope dock��
		&& infoDock == HTNOWHERE)	//����output dock ��
	{
		SetFocus();
		//printf("set focus!\n");
	}
	else{ // ���keys
		
		
	}
}
void CALLBACK Cmfc_test2View:: TimeProc(HWND hwnd, UINT uint, UINT_PTR uptr, DWORD dword)
{
	//��������һ�� magic��
	wglMakeCurrent(hDC, m_hRC);
	m_camera.RenderScene();
	::SwapBuffers(hDC);
	keyFocus();
}



void Cmfc_test2View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		TimeProc(0, 0, 0, 0);
		break;

	default:
		break;
	}
	CView::OnTimer(nIDEvent);
}


BOOL Cmfc_test2View::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch (pMsg ->message)
	{
	case WM_KEYDOWN:
		m_camera.keyDown(pMsg->wParam);
		//printf("key %d down\n", pMsg->wParam);
		break;
	case WM_KEYUP:
		m_camera.keyUp(pMsg->wParam);
		//printf("key up\n");
		break;
	case WM_LBUTTONDOWN:
		m_camera.keyDown(VK_LBUTTON);
		//printf("left mouse down\n");
		break;
	case WM_LBUTTONUP:
		m_camera.keyUp(VK_LBUTTON);
		//printf("left mouse up\n");
		break;
	case WM_RBUTTONDOWN:
		m_camera.keyDown(VK_RBUTTON);
		//printf("right mouse down\n");
		break;
	case WM_RBUTTONUP:
		m_camera.keyUp(VK_RBUTTON);
		//printf("right mouse up\n");
		break;
	default:
		break;

	}
	return CView::PreTranslateMessage(pMsg);
}
