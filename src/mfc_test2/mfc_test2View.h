
// mfc_test2View.h : Cmfc_test2View ��Ľӿ�
//

#pragma once


class Cmfc_test2View : public CView
{
protected: // �������л�����
	Cmfc_test2View();
	DECLARE_DYNCREATE(Cmfc_test2View)

	HGLRC m_hRC;    //Rendering Context��ɫ������
	//CClientDC* m_pDC;        //Device Context�豸������
	HDC hDC;

	BOOL InitializeOpenGL();    //��ʼ�� OpenGL
	BOOL SetupPixelFormat();    //�������ظ�ʽ
	//void RenderScene();         //���Ƴ���
	void CALLBACK  TimeProc(HWND hwnd, UINT uint, UINT_PTR uptr, DWORD dword);
	void keyFocus(void);
// ����
public:
	Cmfc_test2Doc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~Cmfc_test2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // mfc_test2View.cpp �еĵ��԰汾
inline Cmfc_test2Doc* Cmfc_test2View::GetDocument() const
   { return reinterpret_cast<Cmfc_test2Doc*>(m_pDocument); }
#endif

