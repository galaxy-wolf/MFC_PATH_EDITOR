
// mfc_test2View.h : Cmfc_test2View 类的接口
//

#pragma once


class Cmfc_test2View : public CView
{
protected: // 仅从序列化创建
	Cmfc_test2View();
	DECLARE_DYNCREATE(Cmfc_test2View)

	HGLRC m_hRC;    //Rendering Context着色描述表
	//CClientDC* m_pDC;        //Device Context设备描述表
	HDC hDC;

	BOOL InitializeOpenGL();    //初始化 OpenGL
	BOOL SetupPixelFormat();    //设置像素格式
	//void RenderScene();         //绘制场景
	void CALLBACK  TimeProc(HWND hwnd, UINT uint, UINT_PTR uptr, DWORD dword);
	void keyFocus(void);
// 特性
public:
	Cmfc_test2Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~Cmfc_test2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // mfc_test2View.cpp 中的调试版本
inline Cmfc_test2Doc* Cmfc_test2View::GetDocument() const
   { return reinterpret_cast<Cmfc_test2Doc*>(m_pDocument); }
#endif

