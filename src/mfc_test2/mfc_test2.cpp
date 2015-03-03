
// mfc_test2.cpp : ����Ӧ�ó��������Ϊ��
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
	// �����ļ��ı�׼�ĵ�����
	//ON_COMMAND(ID_FILE_NEW, &mfc_test2App::OnFileNew)
	ON_COMMAND(ID_BUTTON32811, &Cmfc_test2App::OnFileOpen)
	ON_COMMAND(ID_BUTTON32812, &Cmfc_test2App::OnFileSave)
	ON_COMMAND(ID_BUTTON32813, &Cmfc_test2App::OnFileSavePath)
	ON_COMMAND(ID_BUTTON32814, &Cmfc_test2App::OnReLoadScene)
	ON_COMMAND(ID_32815, &Cmfc_test2App::OnOpenHelp)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32813, &Cmfc_test2App::OnUpdateFileSavePath)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_PREVIEW_CAPTURE, OnPreviewCapture)
	ON_COMMAND(ID_BEGIN_CAPTURE, OnBeginCapture)
	ON_COMMAND(ID_PAUSE_CAPTURE, OnPauseCapture)
	ON_COMMAND(ID_ABORT_CAPTURE, OnAbortCapture)
	ON_COMMAND(ID_BUTTON32808, OnZoomIn)
	ON_COMMAND(ID_BUTTON32810, OnZoomOut)
END_MESSAGE_MAP()


// Cmfc_test2App ����

Cmfc_test2App::Cmfc_test2App()
{
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("mfc_test2.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� Cmfc_test2App ����

Cmfc_test2App theApp;
MCamera		  m_camera;


// Cmfc_test2App ��ʼ��

BOOL Cmfc_test2App::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Cmfc_test2Doc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(Cmfc_test2View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}

int Cmfc_test2App::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// Cmfc_test2App ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
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
		m_camera.Ui_ShowInfo("��ʼԤ��\n");
	}
}
void Cmfc_test2App::OnBeginCapture()
{
	// Ԥ����ͣ��
	if (m_camera.GetRunMode() == PREVIEW_MODE && m_camera.GetPause() == TRUE)
	{
		m_camera.SetPreview(TRUE);
		m_camera.SetCapture(FALSE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("����Ԥ��\n");
		return;
	}
	if (m_camera.GetRunMode() == CAPTURE_MODE && m_camera.GetPause() == TRUE)
	{
		m_camera.SetPreview(FALSE);
		m_camera.SetCapture(TRUE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("����¼��");
		return;
	}
	if (m_camera.GetRunMode() != CAPTURE_MODE && m_camera.GetRunMode() != PREVIEW_MODE)
	{
		m_camera.SetRunMode(CAPTURE_MODE);
		m_camera.SetPreview(FALSE);
		m_camera.SetCapture(TRUE);
		m_camera.SetPause(FALSE);
		m_camera.Ui_ShowInfo("��ʼ¼��");
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
		m_camera.Ui_ShowInfo("��ͣ��");
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
		m_camera.Ui_ShowInfo("ǿ���˳�¼��");
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
// Cmfc_test2App �Զ������/���淽��

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
	// ���·��
	CFileDialog dlg(FALSE, _T("txt"), _T("point.txt"),
OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
	// ����m_camera �еĺ�������
	if (dlg.DoModal() == IDOK)
	{
		CString folderName = dlg.GetPathName();
		m_camera.SavePoint(folderName);
	}
}

void Cmfc_test2App::OnFileOpen()
{
	// ���·��
	// ʹ��m_camera �еĺ�����
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
		// ֻ����fopen���ļ�
		m_camera.Load_3d_File(folderName);
	}
}

void Cmfc_test2App::OnOpenHelp()
{
	::ShellExecuteA(NULL,"open",".\\Help.chm",NULL,NULL,SW_SHOW);
}

// Cmfc_test2App ��Ϣ�������



