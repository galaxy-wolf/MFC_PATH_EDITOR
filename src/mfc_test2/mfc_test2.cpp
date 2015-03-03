
// mfc_test2.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "mfc_test2.h"
#include "MainFrm.h"
#include "MCamera.h"

#include "mfc_test2Doc.h"
#include "mfc_test2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MCamera.h"

//#ifdef __cplusplus
//extern "C" 
//#endif
//	FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]}; 
// Cmfc_test2App

BEGIN_MESSAGE_MAP(Cmfc_test2App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Cmfc_test2App::OnAppAbout)
	// 基于文件的标准文档命令
	//ON_COMMAND(ID_FILE_NEW, &mfc_test2App::OnFileNew)
	ON_COMMAND(ID_BUTTON32811, &Cmfc_test2App::OnFileOpen)
	ON_COMMAND(ID_BUTTON32812, &Cmfc_test2App::OnFileSave)
	ON_COMMAND(ID_BUTTON32813, &Cmfc_test2App::OnFileSavePath)
	ON_COMMAND(ID_BUTTON32814, &Cmfc_test2App::OnReLoadScene)
	ON_COMMAND(ID_32815, &Cmfc_test2App::OnOpenHelp)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32813, &Cmfc_test2App::OnUpdateFileSavePath)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_PREVIEW_CAPTURE, OnPreviewCapture)
	ON_COMMAND(ID_BEGIN_CAPTURE, OnBeginCapture)
	ON_COMMAND(ID_PAUSE_CAPTURE, OnPauseCapture)
	ON_COMMAND(ID_ABORT_CAPTURE, OnAbortCapture)
	ON_COMMAND(ID_BUTTON32808, OnZoomIn)
	ON_COMMAND(ID_BUTTON32810, OnZoomOut)
END_MESSAGE_MAP()


// Cmfc_test2App 构造

Cmfc_test2App::Cmfc_test2App()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("mfc_test2.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 Cmfc_test2App 对象

Cmfc_test2App theApp;
MCamera		  m_camera;


// Cmfc_test2App 初始化

BOOL Cmfc_test2App::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Cmfc_test2Doc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(Cmfc_test2View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}

int Cmfc_test2App::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// Cmfc_test2App 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void Cmfc_test2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

#define NORMAL_MODE 1
#define SELECT_MODE 2
#define CAPTURE_MODE 3
#define PREVIEW_MODE 4
void Cmfc_test2App::OnPreviewCapture()
{
	if (m_camera.GetRunMode() != PREVIEW_MODE && m_camera.GetRunMode() != CAPTURE_MODE)
	{
		m_camera.SetRunMode(PREVIEW_MODE);
		m_camera.SetPreview(TRUE);
		m_camera.SetCapture(FALSE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("开始预览\n");
	}
}
void Cmfc_test2App::OnBeginCapture()
{
	// 预览暂停中
	if (m_camera.GetRunMode() == PREVIEW_MODE && m_camera.GetPause() == TRUE)
	{
		m_camera.SetPreview(TRUE);
		m_camera.SetCapture(FALSE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("继续预览\n");
		return;
	}
	if (m_camera.GetRunMode() == CAPTURE_MODE && m_camera.GetPause() == TRUE)
	{
		m_camera.SetPreview(FALSE);
		m_camera.SetCapture(TRUE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("继续录制");
		return;
	}
	if (m_camera.GetRunMode() != CAPTURE_MODE && m_camera.GetRunMode() != PREVIEW_MODE)
	{
		m_camera.SetRunMode(CAPTURE_MODE);
		m_camera.SetPreview(FALSE);
		m_camera.SetCapture(TRUE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("开始录制");
	}

}
void Cmfc_test2App::OnPauseCapture()
{
	if ((m_camera.GetRunMode() == PREVIEW_MODE || 
		m_camera.GetRunMode() == CAPTURE_MODE )
		&& m_camera.GetPause() == FALSE)
	{
		m_camera.SetPreview(FALSE);
		m_camera.SetCapture(TRUE);
		m_camera.SetPause(TRUE);
		m_camera.Ui_ShowInfo("暂停！");
	}
}
void Cmfc_test2App::OnAbortCapture()
{
	if (m_camera.GetRunMode() == PREVIEW_MODE ||
		m_camera.GetRunMode() == CAPTURE_MODE)
	{
		m_camera.SetRunMode(SELECT_MODE);
		m_camera.SetPreview(FALSE);
		m_camera.SetCapture(FALSE);
		m_camera.SetPause(FALSE);
		m_camera.QuitCapture();
		m_camera.Ui_ShowInfo("强制退出录制");
	}
}
void Cmfc_test2App::OnZoomIn()
{
	m_camera.SubLookFromUpY();
}
void Cmfc_test2App::OnZoomOut()
{
	m_camera.AddLookFromUpY();
}
// Cmfc_test2App 自定义加载/保存方法

void Cmfc_test2App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void Cmfc_test2App::LoadCustomState()
{
}

void Cmfc_test2App::SaveCustomState()
{
}

void Cmfc_test2App::OnFileSave()
{
	// 获得路径
	CFileDialog dlg(FALSE, _T("txt"), _T("point.txt"),
OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
	// 是用m_camera 中的函数保存
	if (dlg.DoModal() == IDOK)
	{
		CString folderName = dlg.GetPathName();
		m_camera.SavePoint(folderName);
	}
}

void Cmfc_test2App::OnFileOpen()
{
	// 获得路径
	// 使用m_camera 中的函数打开
	CFileDialog dlg(TRUE, _T("txt"), _T("point.txt"),
OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
	if (dlg.DoModal() == IDOK)
	{
		CString folderName = dlg.GetPathName();
		m_camera.LoadPoint(folderName);
	}
}

void Cmfc_test2App::OnFileSavePath()
{
	
	CFileDialog dlg(FALSE, _T("txt"), _T("path.txt"),
	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
	if (dlg.DoModal() == IDOK)
	{
		CString folderName = dlg.GetPathName();
		m_camera.SavePath(folderName);
	}
}

void Cmfc_test2App::OnUpdateFileSavePath( CCmdUI* pCmdUI )
{
	if (m_camera.GetRunMode() == SELECT_MODE || m_camera.GetRunMode() == NORMAL_MODE)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void Cmfc_test2App::OnReLoadScene()
{
	CFileDialog dlg(TRUE, _T("obj"), _T("model.obj"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
	dlg.m_ofn.lpstrFilter = _T("OBJ FILE (*.obj)\0*.obj\0All File(*.*)\0*.*");
	if (dlg.DoModal() == IDOK)
	{
		CString folderName = dlg.GetPathName();
		// 只能用fopen打开文件
		m_camera.Load_3d_File(folderName);
	}
}

void Cmfc_test2App::OnOpenHelp()
{
	::ShellExecuteA(NULL,"open",".\\Help.chm",NULL,NULL,SW_SHOW);
}

// Cmfc_test2App 消息处理程序



