
// mfc_test2.h : mfc_test2 Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// Cmfc_test2App:
// �йش����ʵ�֣������ mfc_test2.cpp
//

class Cmfc_test2App : public CWinAppEx
{
public:
	Cmfc_test2App();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnPreviewCapture();
	afx_msg void OnBeginCapture();
	afx_msg void OnPauseCapture();
	afx_msg void OnAbortCapture();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSavePath();
	afx_msg void OnReLoadScene();
	afx_msg void OnUpdateFileSavePath(CCmdUI* pCmdUI);
	afx_msg void OnOpenHelp();
	DECLARE_MESSAGE_MAP()
};

extern Cmfc_test2App theApp;
