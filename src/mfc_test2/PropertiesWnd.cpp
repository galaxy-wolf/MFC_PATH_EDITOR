
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "mfc_test2.h"

#include "MCamera.h"
extern MCamera		  m_camera;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define NORMAL_POINT_DESC _T("普通点")
#define ZOOM_POINT_DESC	_T("Zoom点")
#define PARALLEL_VIEW_POINT_DESC	_T("Parallel view 点")
#define ROTATE_VIEW_POINT_DESC	_T("Rotate view 点")
#define STOP_POINT_DESC	_T("STOP点")
#define LINK_POINT_DESC	_T("Link点")

BOOL EnableAddPoint= TRUE;
BOOL EnableAddZoom = FALSE;
BOOL EnableAddStop= FALSE;
BOOL EnableAddParallel = FALSE;
BOOL EnableAddRotate = FALSE;
BOOL EnableAddLink = FALSE;
BOOL EnableDelete = FALSE;
BOOL EnableEdit = FALSE;
BOOL EnableCopy = FALSE;
BOOL EnablePast = FALSE;
BOOL EnableUndo= FALSE;
BOOL EnableRedo= FALSE;
/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	//	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_ADD_POINT, OnAddPoint)
	ON_UPDATE_COMMAND_UI(ID_ADD_POINT, OnUpdateAddPoint)
	ON_COMMAND(ID_EDIT_POINT, OnEditPoint)
	ON_UPDATE_COMMAND_UI(ID_EDIT_POINT, OnUpdateEditPoint)
	ON_COMMAND(ID_DELETE_POINT, OnDeletePoint)
	ON_UPDATE_COMMAND_UI(ID_DELETE_POINT, OnUpdateDeletePoint)
	ON_COMMAND(ID_BUTTON32799, OnAddZoomPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32799, OnUpdateAddZoomPoint)
	ON_COMMAND(ID_BUTTON32801, OnAddParallelPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32801, OnUpdateParallelPoint)
	ON_COMMAND(ID_BUTTON32802, OnAddRotatePoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32802, OnUpdateRotatePoint)
	ON_COMMAND(ID_BUTTON32803, OnAddStopPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32803, OnUpdateStopPoint)
	ON_COMMAND(ID_BUTTON32804, OnCopyCurPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32804, OnUpdateCopy)
	ON_COMMAND(ID_BUTTON32805, OnPastToCurPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32805, OnUpdatePast)
	ON_COMMAND(ID_BUTTON32807, OnAddLinkPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32807, OnUpdateLinkPoint)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32816, OnUpdateUndo)
	ON_COMMAND(ID_BUTTON32816, OnUndo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32817, OnUpdateRedo)
	ON_COMMAND(ID_BUTTON32817, OnRedo)

	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE( AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_CBN_SELCHANGE(1, CPropertiesWnd::OnSelectPointFromCombo) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	//???? there are some problems.
	// I changed m_wndToolBar and w_wndObjectCombo positions
	// 
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_VSCROLL| WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
																					//竖直滚动条
	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("未能创建属性组合 \n");
		return -1;      // 未能创建
	}
	m_wndObjectCombo.AddString(_T("-= 请添加关键点 =-"));
	//int i, t;
	//double h = 3.1415926;
	//CString string;
	//for (i = 1, t=1; i < 100; i++)
	//{
	//	string.Format(_T("Point  %d   (%f, %f, %f)  %d"), i, h*i, h*i*i, h*i*i, t*i*10);
	//	m_wndObjectCombo.AddString(string);
	//}

	m_wndObjectCombo.SetCurSel(0);
	m_camera.setCurPoint(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);


	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}
void CPropertiesWnd::SetButtonStatus()
{
	int pos = m_wndObjectCombo.GetCurSel();
	if (pos == 0)
	{
		EnableEdit = EnableAddZoom = EnableAddParallel 
		= EnableAddRotate = EnableAddStop 
		= EnableAddLink = EnableCopy 
		= EnablePast = EnableDelete 
		= FALSE;
		EnableAddPoint = TRUE;
		return;
	}

	EnableAddPoint = EnableAddZoom = EnableAddParallel 
	= EnableAddRotate = EnableAddStop 
	= EnableAddLink = EnableCopy 
	= EnableDelete 
	= TRUE;
	
	PointInfo p = m_camera.GetPointDetail(pos);
	int pType = p.PointType;
	switch(pType)
	{
	case NORMAL_POINT:
		EnableEdit = TRUE;
		EnablePast = TRUE;
		break;
	case ZOOM_POINT:
	case PARALLEL_VIEW_POINT:
	case ROTATE_VIEW_POINT:
		EnablePast = FALSE;
		EnableEdit = TRUE;
		break;
	case STOP_POINT:
	case LINK_POINT:
		EnableEdit = FALSE;
		EnablePast = FALSE;
		break;
	}
}
void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
	/*static int i;
	char  ch[100];
	sprintf(ch, "%d", i++);
	m_camera.Ui_ShowInfo(ch);*/
}


void CPropertiesWnd::UpdatePropUiAfterAddPoint(unsigned int addPos)
{
	int size = m_camera.GetPointsNum();
	m_wndObjectCombo.ResetContent();
	m_wndObjectCombo.AddString(_T("-= 请添加关键点 =-"));
	for (int i = 1; i <= size; i++)
		UpdateCombox(i);
	m_wndObjectCombo.SetCurSel(addPos);
	m_camera.setCurPoint(addPos);

	SetButtonStatus();

	ShowPointInfo(addPos);
}
void CPropertiesWnd::OnAddPoint()
{
	if(m_camera.GetCanSelectFlag() == TRUE 
		&& m_camera.GetSelectPointType() == NORMAL_POINT)
	{
		m_camera.setCanSelectFlag(FALSE);
		SetButtonStatus();
		return;
	}

	EnableEdit = EnableAddZoom = EnableAddParallel 
	= EnableAddRotate = EnableAddStop 
	= EnableAddLink = EnableCopy 
	= EnablePast = EnableDelete 
	= FALSE;

	m_camera.setCanSelectFlag(TRUE);
	m_camera.setSelectPointType(NORMAL_POINT);
	m_camera.SetRunMode(SELECT_MODE);
	printf("add Normal point \n");
}
void CPropertiesWnd::OnUpdateAddPoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableAddPoint);
}
void CPropertiesWnd::OnEditPoint()
{
	// TODO: 在此处添加命令处理程序代码
	if (m_camera.GetCurPoint() < 1 || m_camera.GetCurPoint() > m_camera.GetPointsNum())
	{
		return;
	}
	m_camera.BackUpCurPoint();
	m_camera.SetRunMode(CHANGE_CENTER_MODE);
	m_camera.initChangeCenter();
}
void CPropertiesWnd::OnUpdateEditPoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableEdit);
}
void CPropertiesWnd::OnDeletePoint()
{
	
	int pos;
	pos = m_wndObjectCombo.GetCurSel();
	if ( pos > 0)
	{
		m_camera.DeletePoint(pos);
		m_wndObjectCombo.DeleteString(pos);
		int size = m_camera.GetPointsNum();
		/*sprintf(ch, "point num is %d", size);
		m_camera.Ui_ShowInfo(ch);*/
		for (int i = pos; i <= size; i++)
			UpdateCombox(i);
		m_wndObjectCombo.SetCurSel(pos - 1);
		m_camera.setCurPoint(pos - 1);
		ShowPointInfo(pos - 1);
		SetButtonStatus();
	}
}
void CPropertiesWnd::OnUpdateDeletePoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableDelete);
}
void CPropertiesWnd::OnAddZoomPoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		return;
	}
	if (m_camera.GetCanSelectFlag() == TRUE && m_camera.GetSelectPointType() == ZOOM_POINT)
	{
		m_camera.setCanSelectFlag(FALSE);
		SetButtonStatus();
		return;
	}
	EnableEdit = EnableAddPoint = EnableAddParallel 
		= EnableAddRotate = EnableAddStop 
		= EnableAddLink = EnableCopy 
		= EnablePast = EnableDelete 
		= FALSE;
	EnableAddZoom = TRUE;

	m_camera.setCanSelectFlag(TRUE);
	m_camera.setSelectPointType(ZOOM_POINT);
	m_camera.SetRunMode(SELECT_MODE);
	printf("add zoom point \n");
}
void CPropertiesWnd::OnUpdateAddZoomPoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableAddZoom);
}
void CPropertiesWnd::OnAddParallelPoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		return;
	}
	if(m_camera.GetCanSelectFlag() == TRUE && m_camera.GetSelectPointType() == PARALLEL_VIEW_POINT)
	{
		m_camera.setCanSelectFlag(FALSE);
		SetButtonStatus();
		return;
	}
	EnableEdit = EnableAddPoint = EnableAddZoom
		= EnableAddRotate = EnableAddStop 
		= EnableAddLink = EnableCopy 
		= EnablePast = EnableDelete 
		= FALSE;
	EnableAddParallel = TRUE;

	m_camera.setCanSelectFlag(TRUE);
	m_camera.setSelectPointType(PARALLEL_VIEW_POINT);
	m_camera.SetRunMode(SELECT_MODE);
	printf("add Parallel point \n");
}
void CPropertiesWnd::OnUpdateParallelPoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableAddParallel);
}
void CPropertiesWnd::OnAddRotatePoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		return;
	}
	PointInfo p;
	int pos = m_wndObjectCombo.GetCurSel();
	p = m_camera.GetPointDetail(pos);
	p.cf = 100;
	p.linkNum = 0;
	p.linkTo = 0;
	p.PointType = ROTATE_VIEW_POINT;
	m_camera.AddPoint(pos + 1, p);
	UpdatePropUiAfterAddPoint(pos + 1);
	// auto go into change_center_mode;
	//m_camera.SetRunMode(CHANGE_CENTER_MODE);
	//m_camera.initChangeCenter();
	printf("add Rotate point \n");
}
void CPropertiesWnd::OnUpdateRotatePoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableAddRotate);
}
void CPropertiesWnd::OnAddStopPoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		return;
	}
	PointInfo p;
	int pos = m_wndObjectCombo.GetCurSel();
	p = m_camera.GetPointDetail(pos);
	p.cf = 100;
	p.linkNum = 0;
	p.linkTo = 0;
	p.PointType = STOP_POINT;
	m_camera.AddPoint(pos + 1, p);
	UpdatePropUiAfterAddPoint(pos + 1);
	printf("add Stop point \n");
}
void CPropertiesWnd::OnUpdateStopPoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableAddRotate);
}
void CPropertiesWnd::OnAddLinkPoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
	{
		return;
	}
	PointInfo p;
	int pos = m_wndObjectCombo.GetCurSel();
	p.linkTo = p.linkNum = 0;
	p.PointType = LINK_POINT;
	m_camera.AddPoint(pos + 1, p);
	UpdatePropUiAfterAddPoint(pos + 1);
	printf("add link point \n");
}
void CPropertiesWnd::OnUpdateLinkPoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableAddLink);
}
void CPropertiesWnd::OnCopyCurPoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
		return;
	int pos = m_wndObjectCombo.GetCurSel();
	m_camera.CopyToClipBoard(m_camera.GetPointDetail(pos));
	printf("Copy point \n");
}
void CPropertiesWnd::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnableCopy);
}
void CPropertiesWnd::OnPastToCurPoint()
{
	if (m_wndObjectCombo.GetCurSel() == 0)
		return;
	int pos = m_wndObjectCombo.GetCurSel();
	PointInfo p, curP;
	p = m_camera.GetClipBoard();
	curP = m_camera.GetPointDetail(pos);
	curP.center = p.center;
	curP.eye = p.eye;
	curP.up = p.up;
	// 只有普通点可以粘贴
	if (curP.PointType == NORMAL_POINT)
		m_camera.ChangePointInfo(pos, curP);
	ShowPointInfo(pos);
	UpdateCombox(pos);
	m_wndObjectCombo.SetCurSel(pos);
	
	printf("Past point \n");
}
void CPropertiesWnd::OnUpdatePast(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(EnablePast);
}
void CPropertiesWnd::OnUndo()
{
	// mcamer->undo();
	// update ui;
	// set curPoint mcamer.undoPoint();
	int pos = m_camera.undo();
	if (pos < 0)
		return;
	UpdatePropUiAfterAddPoint(pos);

	printf("undo\n");
}
void CPropertiesWnd::OnUpdateUndo(CCmdUI* pCmdUI)
{
	EnableUndo = m_camera.CanUndo();
	pCmdUI->Enable(EnableUndo);
}
void CPropertiesWnd::OnRedo()
{
	int pos = m_camera.redo();
	if (pos < 0)
		return;
	UpdatePropUiAfterAddPoint(pos);
	printf("Redo\n");
}
void CPropertiesWnd::OnUpdateRedo(CCmdUI* pCmdUI)
{
	EnableRedo = m_camera.CanRedo();
	pCmdUI->Enable(EnableRedo);
}
void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	//CMFCPropertyGridProperty* pSize;
	//CMFCPropertyGridProperty* pProp;

	//视点位置
	propList [0][0] = new CMFCPropertyGridProperty(_T("Eye"), 1, TRUE);
	propList [0][0]->AllowEdit(FALSE);
	propList [0][1] = new CMFCPropertyGridProperty(_T("X轴"), (_variant_t) 0.0000, _T("视点位置的X坐标"), 11);
	propList [0][0]->AddSubItem(propList[0][1]);

	propList [0][2] = new CMFCPropertyGridProperty(_T("Y轴"), (_variant_t) 0.0000, _T("视点位置的Y坐标"),12);
	propList [0][0] ->AddSubItem(propList[0][2]);
	propList [0][3] = new CMFCPropertyGridProperty( _T("Z轴"), (_variant_t) 0.0000, _T("视点位置的Z坐标"),13);
	propList [0][0] ->AddSubItem(propList[0][3]);

	m_wndPropList.AddProperty(propList[0][0]);
	
	// center
	propList [1][0] = new CMFCPropertyGridProperty(_T("Center"), 2, TRUE);
	propList [1][0] ->AllowEdit(FALSE);
	propList [1][1] = new CMFCPropertyGridProperty(_T("X轴"), (_variant_t) 0.0, _T("焦点的X坐标"),21);
	//pProp->EnableSpinControl(TRUE, (_variant_t)-99.0, (_variant_t)9999);
	propList [1][0] ->AddSubItem(propList[1][1]);

	propList [1][2] = new CMFCPropertyGridProperty( _T("Y轴"), (_variant_t) 0.0, _T("焦点的Y坐标"), 22);
	propList [1][0] ->AddSubItem(propList[1][2]);

	propList [1][3] = new CMFCPropertyGridProperty( _T("Z轴"), (_variant_t) 0.0, _T("焦点的Z坐标"),23);
	propList [1][0] ->AddSubItem(propList[1][3]);

	m_wndPropList.AddProperty(propList[1][0]);
	//
	// up
	propList [2][0] = new CMFCPropertyGridProperty(_T("UP"), 3, TRUE);

	propList [2][0]->AllowEdit(FALSE);
	propList [2][1]= new CMFCPropertyGridProperty(_T("X轴"), (_variant_t) 0.0, _T("相机朝向的X坐标"), 31);
	propList [2][0]->AddSubItem(propList[2][1]);

	propList [2][2]= new CMFCPropertyGridProperty( _T("Y轴"), (_variant_t) 0.0, _T("相机朝向的Y坐标"), 32);
	propList [2][0]->AddSubItem(propList[2][2]);

	propList [2][3]= new CMFCPropertyGridProperty( _T("Z轴"), (_variant_t) 0.0, _T("相机朝向的Z坐标"), 33);
	propList [2][0]->AddSubItem(propList[2][3]);

	m_wndPropList.AddProperty(propList[2][0]);

	//// Time
	//propList [3][0]= new  CMFCPropertyGridProperty(_T("Time"), /*(_variant_t)*/ (long)0, _T("相机到达这个点的时间，单位为ms"), 4);
	////propList [3][0]->EnableSpinControl(TRUE, 0, 999999999);
	//m_wndPropList.AddProperty(propList[3][0]);
	// 总的Frame
	propList [5][0] = new CMFCPropertyGridProperty(_T("FrameId"), (long)0, 
		_T("录制到该点时的帧数"), 6);
	m_wndPropList.AddProperty((propList[5][0]));
	// 与前一点之间的Frame
	propList [6][0] = new CMFCPropertyGridProperty(_T("cf"), (long)0,
		_T("该值等于 此点FrameId - 上一点FrameId"), 7);
	m_wndPropList.AddProperty((propList[6][0]));
	// 点的类型 普通， Zoom ， Parallel， Rotate，Stop 等
	propList [7][0] = new CMFCPropertyGridProperty( _T("点的类型"), (_variant_t) _T("普通点"),
		_T("点的类型包括普通点，Zoom， Parallel， Rotate， stop等"), 8);
	propList[7][0]->AddOption(NORMAL_POINT_DESC);
	/*propList[7][0]->AddOption(ZOOM_POINT_DESC);
	propList[7][0]->AddOption(PARALLEL_VIEW_POINT_DESC);
	propList[7][0]->AddOption(ROTATE_VIEW_POINT_DESC);
	propList[7][0]->AddOption(STOP_POINT_DESC);
	propList[7][0]->AddOption(LINK_POINT_DESC);*/
	propList [7][0]->AllowEdit(FALSE);
	m_wndPropList.AddProperty((propList[7][0]));
	propList [8][0] = new CMFCPropertyGridProperty( _T("LinkTo"), (long)0,
		_T("仅在LINK_POINT 类型点中有效， 表示此点的eye， center， up 由LinkTo所指向的点决定"), 9);
	m_wndPropList.AddProperty((propList [8][0]));
	

	propList [9][0] = new CMFCPropertyGridProperty(_T("透视参数"), 1, TRUE);
	propList [9][0]->AllowEdit(FALSE);
	propList [9][1] = new CMFCPropertyGridProperty(_T("fovy"), (_variant_t) m_camera.GetFovey(), _T("fovy"), 101);
	propList [9][0]->AddSubItem(propList[9][1]);

	propList [9][2] = new CMFCPropertyGridProperty(_T("zNear"), (_variant_t) m_camera.GetZNear(), _T("zNear"),102);
	propList [9][0] ->AddSubItem(propList[9][2]);
	propList [9][3] = new CMFCPropertyGridProperty( _T("zFar"), (_variant_t) m_camera.GetZFar(), _T("zFar"),103);
	propList [9][0] ->AddSubItem(propList[9][3]);

	m_wndPropList.AddProperty(propList[9][0]);
	// Y 平面
	propList [4][0]= new  CMFCPropertyGridProperty(_T("选点平面"), (_variant_t) 0.0, _T("选点平面的Y 坐标"), 5);
	m_wndPropList.AddProperty(propList[4][0]);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}
// 改变点的信息。
LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam; 
	//// 修改示例
	//if (m_wndObjectCombo.GetCurSel() == 0 && pProp->GetData() != 5)
	//{
	//	ShowPointInfo(0); 
	//	return TRUE;
	//}
	if ( wParam != m_wndPropList.GetDlgCtrlID())
		return TRUE;
	PointInfo p, p1;
	int pos;

	pos = m_wndObjectCombo.GetCurSel();
	p = m_camera.GetPointDetail(pos);

	// 对于特殊点 不允许通过输入数值改变参数
	if (p.PointType != NORMAL_POINT)
	{
		switch (pProp->GetData())
		{
		case 11:
		case 12:
		case 13:
		case 21:
		case 22:
		case 23:
		case 31:
		case 32:
		case 33:
			ShowPointInfo(pos);
			return TRUE;
			break;
		default:
			break;
		}
	}
	//printf("property changed\n");
	COleVariant var = pProp-> GetValue();
	
	switch (pProp->GetData())
	{
	case 1:
		break;
	case 11:
		p.eye.x = var.dblVal;
		break;
	case 12:
		p.eye.y = var.dblVal;
		break;
	case 13:
		p.eye.z = var.dblVal;
		break;
	case 2:
		break;
	case 21:
		p.center.x = var.dblVal;
		break;
	case 22:
		p.center.y = var.dblVal;
		break;
	case 23:
		p.center.z = var.dblVal;
		break;
	case 3:
		break;
	case 31:
		p.up.x = var.dblVal;
		break;
	case 32:
		p.up.y = var.dblVal;
		break;
	case 33:
		p.up.z = var.dblVal;
		break;
	case 5: // MsehY
		m_camera.setMeshY(var.dblVal);
		return TRUE;
		break;
	case 6://FrameId
		if (pos == 1)
			break;
		p.cf = var.lVal - m_camera.GetFrameId(pos - 1);
		if (p.cf < 1)
			p.cf = 1;
		break;
	case 7:// Frame between two point
		if (pos == 1)
			break;
		p.cf = var.lVal;
		if (p.cf < 1)
			p.cf = 1;
		break;
	case 8:// PointType 
		if (pos == 1) // 1# 只能是NORMAL 不用再处理了
			return TRUE;
		if (p.PointType == LINK_POINT && var != (COleVariant)LINK_POINT_DESC)
		{
			m_camera.deleteLink(p.linkTo);
			p.linkTo = 0;
		}
		// 为了简便 只能从特殊点退化为NORMAL 点
		if (var == (COleVariant)NORMAL_POINT_DESC)
			p.PointType = NORMAL_POINT;
		else
		{
		//	nothing 
		}
		break;
	case 9:// linkTo
									// 不可link 之后的点或自己
		if (p.PointType == LINK_POINT && var.lVal < pos && var.lVal > 0)
		{
			if (p.linkTo > 0 && p.linkTo < pos)
			{
				m_camera.deleteLink(p.linkTo);
			}
			p1 = m_camera.GetPointDetail(var.lVal);
			m_camera.addLink(var.lVal);
			p.linkTo = var.lVal;
			p.eye = p1.eye;
			p.center = p1.center;
			p.up = p1.up;
		}
		else
		{// show error ??? box show

			//return TRUE;
		}

		break;
	case 101:
		if (var.dblVal > 0 && var.dblVal < 90)
			m_camera.SetPerspective(var.dblVal, m_camera.GetZNear(), m_camera.GetZFar());
		else
			ShowPerspective();
		return TRUE;
		break;
	case 102:
		if (var.dblVal > 0 && var.dblVal < m_camera.GetZFar())
			m_camera.SetPerspective(m_camera.GetFovey(), var.dblVal, m_camera.GetZFar());
		else
			ShowPerspective();
		return TRUE;
		break;
	case 103:
		if (var.dblVal > 0 && var.dblVal > m_camera.GetZNear())
			m_camera.SetPerspective(m_camera.GetFovey(), m_camera.GetZNear(), var.dblVal);
		else
			ShowPerspective();
		return TRUE;
		break;
	default:
		break;
	}
	// 修改示例
	if (m_wndObjectCombo.GetCurSel() == 0 && pProp->GetData() != 5)
	{
		ShowPointInfo(0); 
		return TRUE;
	}
	m_camera.ChangePointInfo(pos, p);
	int size = m_camera.GetPointsNum();
	for (int i = pos; i <= size; i++)
		UpdateCombox(i);
	m_wndObjectCombo.SetCurSel(pos);
	m_camera.setCurPoint(pos);
	ShowPointInfo(pos);
	return TRUE;
}

void CPropertiesWnd::OnSelectPointFromCombo()
{
	ShowPointInfo(m_wndObjectCombo.GetCurSel());
	m_camera.setCurPoint(m_wndObjectCombo.GetCurSel());
	if (m_camera.GetRunMode() == SELECT_MODE)
	{
		m_camera.LookCurPointFromUp();
	}
	SetButtonStatus();
}

void CPropertiesWnd::ShowPointInfo(unsigned int pointNo)
{
	PointInfo p = m_camera.GetPointDetail(pointNo);
	propList[0][1]->SetValue(p.eye.x);
	propList[0][2]->SetValue(p.eye.y);
	propList[0][3]->SetValue(p.eye.z);
	propList[1][1]->SetValue(p.center.x);
	propList[1][2]->SetValue(p.center.y);
	propList[1][3]->SetValue(p.center.z);
	propList[2][1]->SetValue(p.up.x);
	propList[2][2]->SetValue(p.up.y);
	propList[2][3]->SetValue(p.up.z);
	// long 
	propList[5][0]->SetValue((long)m_camera.GetFrameId(pointNo));
	propList[6][0]->SetValue((long)p.cf);
	COleVariant var;
	switch(p.PointType) // removeall Option an add Normal and 
	{
	case NORMAL_POINT:
		var = (COleVariant)NORMAL_POINT_DESC;
		break;
	case ZOOM_POINT:
		var = (COleVariant)ZOOM_POINT_DESC;
		break;
	case PARALLEL_VIEW_POINT:
		var = (COleVariant)PARALLEL_VIEW_POINT_DESC;
		break;
	case ROTATE_VIEW_POINT:
		var = (COleVariant)ROTATE_VIEW_POINT_DESC;
		break;
	case STOP_POINT:
		var = (COleVariant)STOP_POINT_DESC;
		break;
	case LINK_POINT:
		var = (COleVariant)LINK_POINT_DESC;
		break;
	default:
		break;
	}
	propList[7][0]->SetValue(var);
	propList[8][0]->SetValue((long)p.linkTo);
	UpdateCombox(pointNo);
	m_wndObjectCombo.SetCurSel(pointNo);
}
void CPropertiesWnd::ShowMeshY(GLdouble y)
{
	propList[4][0]->SetValue(y);
}
void CPropertiesWnd::ShowPerspective(void)
{
	propList[9][1]->SetValue(m_camera.GetFovey());
	propList[9][2]->SetValue(m_camera.GetZNear());
	propList[9][3]->SetValue(m_camera.GetZFar());
}
void CPropertiesWnd::UpdateCombox(unsigned int pointNo)
{
	if (pointNo == 0)
		return;
	CString s;
	PointInfo p;
	p = m_camera.GetPointDetail(pointNo);
	
	s.Format(_T("POINT %d (%f, %f, %f)  %d %c"), pointNo, p.eye.x, p.eye.y, 
		p.eye.z,  m_camera.GetFrameId(pointNo), p.PointType);
	if (pointNo > (m_wndObjectCombo.GetCount()))
	{//直接插到最后
		m_wndObjectCombo.AddString(s);
	}
	else
	{
		m_wndObjectCombo.DeleteString(pointNo);
		m_wndObjectCombo.InsertString(pointNo, s);
	}

}
