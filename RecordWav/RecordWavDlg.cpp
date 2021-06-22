
// RecordWavDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordWav.h"
#include "RecordWavDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CRecordWavDlg 对话框



CRecordWavDlg::CRecordWavDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRecordWavDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRecordWavDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRecordWavDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_BN_CLICKED(IDC_BTN_RECORD_START, &CRecordWavDlg::OnBnClickedBtnRecordStart)
	ON_BN_CLICKED(IDC_BTN_RECORD_STOP, &CRecordWavDlg::OnBnClickedBtnRecordStop)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CRecordWavDlg::OnBnClickedBtnPlay)

	ON_MESSAGE(MM_WIM_OPEN, &CRecordWavDlg::OnMmWimOpen)
	ON_MESSAGE(MM_WIM_DATA, &CRecordWavDlg::OnMmWimData)
	ON_MESSAGE(MM_WIM_CLOSE, &CRecordWavDlg::OnMmWimClose)
	ON_MESSAGE(MM_WOM_OPEN, &CRecordWavDlg::OnMmWomOpen)
	ON_MESSAGE(MM_WOM_DONE, &CRecordWavDlg::OnMmWomDone)
	ON_MESSAGE(MM_WOM_CLOSE, &CRecordWavDlg::OnMmWomClose)
END_MESSAGE_MAP()


// CRecordWavDlg 消息处理程序

BOOL CRecordWavDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	/*m_dwDataLength = FRAGMENT_SIZE;

	//allocate memory for wave header
	m_pWaveHdr1 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	m_pWaveHdr2 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));

	//allocate memory for save buffer
	m_pSaveBuffer = reinterpret_cast<PBYTE>(malloc(1));*/

	m_RecordWavImpl.Init(this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRecordWavDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRecordWavDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRecordWavDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRecordWavDlg::OnBnClickedBtnRecordStart()
{
	m_RecordWavImpl.RecordStart();
}


void CRecordWavDlg::OnBnClickedBtnRecordStop()
{
	m_RecordWavImpl.RecordStop();
}


void CRecordWavDlg::OnBnClickedBtnPlay()
{
	m_RecordWavImpl.RecordPlay();
}


afx_msg LRESULT CRecordWavDlg::OnMmWimOpen(WPARAM wParam, LPARAM lParam)
{
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(FALSE);
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWimData(WPARAM wParam, LPARAM lParam)
{
	if (!m_RecordWavImpl.OnMmWimData(wParam, lParam))
		return TRUE;

	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWimClose(WPARAM wParam, LPARAM lParam)
{
	int ret = m_RecordWavImpl.OnMmWimClose(wParam, lParam);
	if (ret == 0)
		return TRUE;
	else if (ret == 1)
	{
		//enable play
		((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
		((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
		((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(TRUE);
	}
	else
	{
		((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
		((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	}
	
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomOpen(WPARAM wParam, LPARAM lParam)
{
	m_RecordWavImpl.OnMmWomOpen(wParam, lParam);

	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(FALSE);
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomDone(WPARAM wParam, LPARAM lParam)
{
	m_RecordWavImpl.OnMmWomDone(wParam, lParam);
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomClose(WPARAM wParam, LPARAM lParam)
{
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(TRUE);
	return 0;
}
