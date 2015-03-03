
// mfc_test2.h : mfc_test2 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// Cmfc_test2App:
// 有关此类的实现，请参阅 mfc_test2.cpp
//

class Cmfc_test2App : public CWinAppEx
{
public:
	Cmfc_test2App();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
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
