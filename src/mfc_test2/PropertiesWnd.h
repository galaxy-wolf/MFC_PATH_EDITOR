
#pragma once
class PointInfo;
class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
// 构造
public:
	CPropertiesWnd();

	void AdjustLayout();

// 特性
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

//protected:
public:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;
	CSpinButtonCtrl m_spin;

// 实现
public:
	virtual ~CPropertiesWnd();

	afx_msg void OnAddPoint();
	void ShowPointInfo(unsigned int pointNO);
	void UpdateCombox(unsigned int pointNo);
	void ShowMeshY(GLdouble y);
	void ShowPerspective(void);
	void UpdatePropUiAfterAddPoint(unsigned int AddPos);
	void SetButtonStatus(void);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddPoint(CCmdUI* pCmdUI);
	afx_msg void OnEditPoint();
	afx_msg void OnUpdateEditPoint(CCmdUI* pCmdUI);
	afx_msg void OnAddZoomPoint();
	afx_msg void OnUpdateAddZoomPoint(CCmdUI* pCmdUI);
	afx_msg void OnAddParallelPoint();
	afx_msg void OnUpdateParallelPoint(CCmdUI* pCmdUI);
	afx_msg void OnAddRotatePoint();
	afx_msg void OnUpdateRotatePoint(CCmdUI* pCmdUI);
	afx_msg void OnAddStopPoint();
	afx_msg void OnUpdateStopPoint(CCmdUI* pCmdUI);
	afx_msg void OnAddLinkPoint();
	afx_msg void OnUpdateLinkPoint(CCmdUI* pCmdUI);
	afx_msg void OnCopyCurPoint();
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnPastToCurPoint();
	afx_msg void OnUndo();
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnRedo();
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePast(CCmdUI* pCmdUI);
	afx_msg void OnDeletePoint();
	afx_msg void OnUpdateDeletePoint(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSelectPointFromCombo();
	afx_msg LRESULT OnPropertyChanged (WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
private:
	CMFCPropertyGridProperty* propList[16][4];
};

