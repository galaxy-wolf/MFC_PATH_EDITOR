
// mfc_test2View.cpp : Cmfc_test2View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
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

// Cmfc_test2View 构造/析构

Cmfc_test2View::Cmfc_test2View()
{
	// TODO: 在此处添加构造代码
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

// Cmfc_test2View 绘制

void Cmfc_test2View::OnDraw(CDC* /*pDC*/)
{
	Cmfc_test2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// Cmfc_test2View 打印


void Cmfc_test2View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cmfc_test2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void Cmfc_test2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void Cmfc_test2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// Cmfc_test2View 诊断

#ifdef _DEBUG
void Cmfc_test2View::AssertValid() const
{
	CView::AssertValid();
}

void Cmfc_test2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cmfc_test2Doc* Cmfc_test2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmfc_test2Doc)));
	return (Cmfc_test2Doc*)m_pDocument;
}
#endif //_DEBUG


// Cmfc_test2View 消息处理程序
BOOL Cmfc_test2View::SetupPixelFormat(void)
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // pfd结构的大小 
		1,                              // 版本号 
		PFD_DRAW_TO_WINDOW |            // 支持在窗口中绘图 
		PFD_SUPPORT_OPENGL |            // 支持 OpenGL 
		PFD_DOUBLEBUFFER,               // 双缓存模式 
		PFD_TYPE_RGBA,                  // RGBA 颜色模式 
		32,                             // 32 位颜色深度 
		0, 0, 0, 0, 0, 0,               // 忽略颜色位 
		0,                              // 没有非透明度缓存 
		0,                              // 忽略移位位 
		0,                              // 无累计缓存 
		0, 0, 0, 0,                     // 忽略累计位 
		16,                             // 32 位深度缓存     
		0,                              // 无模板缓存 
		0,                              // 无辅助缓存 
		PFD_MAIN_PLANE,                 // 主层 
		0,                              // 保留 
		0, 0, 0                         // 忽略层,可见性和损毁掩模 

	};
	int pixelFormat;
	// 为设备描述表得到最匹配的像素格式 
	if((pixelFormat = ChoosePixelFormat(/*m_pDC->GetSafeHdc()*/ hDC, &pfd)) == 0)
	{
		MessageBox( _T("ChoosePixelFormat failed") );
		return FALSE;
	}
	// 设置最匹配的像素格式为当前的像素格式 
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
	
	// 设置当前的绘图像素格式
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
	// 使绘图描述表为当前调用现程的当前绘图描述表
	if( wglMakeCurrent(/*m_pDC->GetSafeHdc()*/ hDC, m_hRC) == FALSE)
	{
		MessageBox(_T("Error making RC Current"));
		return FALSE;
	}

	// 创建绘图描述表
	
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

	// TODO:  在此添加您专用的创建代码
	// 
	if (!InitializeOpenGL())
		return -1;
	SetTimer(1, 33, NULL);
	return 0;
}


void Cmfc_test2View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	static int count=0;
	GLdouble aspect_ratio; 
	 printf("%d : %d,  %d\n",count++, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	if (cy == 0)
		cy = 1;
	m_camera.SetCx(cx);
	m_camera.SetCy(cy);
	glViewport(0, 0, cx, cy);
	aspect_ratio = (GLdouble)cx / (GLdouble)cy;
	
}


BOOL Cmfc_test2View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}


void Cmfc_test2View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
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

//鼠标在视图上时， 自动将输入指向视图
void Cmfc_test2View::keyFocus(void)
{
	// 效率 ？？？ 每次刷新界面都会检测
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
		&& properDock == HTNOWHERE	//不在 prope dock上
		&& infoDock == HTNOWHERE)	//不在output dock 上
	{
		SetFocus();
		//printf("set focus!\n");
	}
	else{ // 清空keys
		
		
	}
}
void CALLBACK Cmfc_test2View:: TimeProc(HWND hwnd, UINT uint, UINT_PTR uptr, DWORD dword)
{
	//必须有这一句 magic；
	wglMakeCurrent(hDC, m_hRC);
	m_camera.RenderScene();
	::SwapBuffers(hDC);
	keyFocus();
}



void Cmfc_test2View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	// TODO: 在此添加专用代码和/或调用基类
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
