
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
	m_pBuffer1 = nullptr;
	m_pBuffer2 = nullptr;
	m_pSaveBuffer = nullptr;
	m_pNewBuffer = nullptr;
	m_pWaveHdr1 = nullptr;
	m_pWaveHdr2 = nullptr;
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
	ON_WM_DESTROY()
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

	m_dwDataLength = FRAGMENT_SIZE;

	//allocate memory for wave header
	m_pWaveHdr1 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	m_pWaveHdr2 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));

	//allocate memory for save buffer
	m_pSaveBuffer = reinterpret_cast<PBYTE>(malloc(1));

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

void CRecordWavDlg::Release()
{
	if (m_pBuffer1)
	{
		free(m_pBuffer1);
		m_pBuffer1 = nullptr;
	}
	if (m_pBuffer2)
	{
		free(m_pBuffer2);
		m_pBuffer2 = nullptr;
	}
}

void CRecordWavDlg::OnBnClickedBtnRecordStart()
{
	Release();

	// allocate buffer memory
	m_pBuffer1 = (PBYTE)malloc(m_dwDataLength);
	m_pBuffer2 = (PBYTE)malloc(m_dwDataLength);
	if (!m_pBuffer1 || !m_pBuffer2)
	{
		Release();
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Memory erro!"));
		return;
	}

	// open waveform audio for input
	m_waveform.wFormatTag = WAVE_FORMAT_PCM;
	m_waveform.nChannels = 2;
	m_waveform.nSamplesPerSec = 44100;
	m_waveform.nAvgBytesPerSec = 176400;
	m_waveform.nBlockAlign = 4;
	m_waveform.wBitsPerSample = 16;
	m_waveform.cbSize = 0;
	
	if (waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveform, (DWORD)this->m_hWnd, NULL, CALLBACK_WINDOW))
	{
		Release();
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Audio can not be open!"));
	}

	m_pWaveHdr1->lpData = (LPSTR)m_pBuffer1;
	m_pWaveHdr1->dwBufferLength = m_dwDataLength;
	m_pWaveHdr1->dwBytesRecorded = 0;
	m_pWaveHdr1->dwUser = 0;
	m_pWaveHdr1->dwFlags = 0;
	m_pWaveHdr1->dwLoops = 1;
	m_pWaveHdr1->lpNext = NULL;
	m_pWaveHdr1->reserved = 0;
	
	waveInPrepareHeader(m_hWaveIn, m_pWaveHdr1, sizeof(WAVEHDR));
	
	m_pWaveHdr2->lpData = (LPSTR)m_pBuffer2;//
	m_pWaveHdr2->dwBufferLength = m_dwDataLength;
	m_pWaveHdr2->dwBytesRecorded = 0;
	m_pWaveHdr2->dwUser = 0;
	m_pWaveHdr2->dwFlags = 0;
	m_pWaveHdr2->dwLoops = 1;
	m_pWaveHdr2->lpNext = NULL;
	m_pWaveHdr2->reserved = 0;
	
	waveInPrepareHeader(m_hWaveIn, m_pWaveHdr2, sizeof(WAVEHDR));
	
	m_pSaveBuffer = (PBYTE)realloc (m_pSaveBuffer, 1);
	
	// Add the buffers
	waveInAddBuffer (m_hWaveIn, m_pWaveHdr1, sizeof (WAVEHDR));
	waveInAddBuffer (m_hWaveIn, m_pWaveHdr2, sizeof (WAVEHDR));
	
	// Begin sampling
	m_bEnding = FALSE;
	m_dwDataLength = 0;
	waveInStart (m_hWaveIn);
}


void CRecordWavDlg::OnBnClickedBtnRecordStop()
{
	m_bEnding = TRUE;
	// 停止录音
	waveInReset(m_hWaveIn);
	
	// 存储声音文件
	CFile m_file;
	CFileException fileException;
	SYSTEMTIME sys2; // 获取系统时间确保文件的保存不出现重名
	GetLocalTime(&sys2);

	// 以下实现将录入的声音转换为wave格式文件
	// 查找当前目录中有没有Voice文件夹 没有就先创建一个，有就直接存储
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	// 获取exe目录
	CString PROGRAM_PATH = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	// Debug目录下RecordVoice文件夹中
	PROGRAM_PATH += _T("RecordVoice\\");
	
	if (!(GetFileAttributes(PROGRAM_PATH) == FILE_ATTRIBUTE_DIRECTORY))
	{
		if (!CreateDirectory(PROGRAM_PATH,NULL))
		{
			AfxMessageBox(_T("Make Dir Error"));
		}
	}
	
	//kn_string strFilePath = _T("RecordVoice\\");
	//GetFilePath(strFilePath);
	CString m_csFileName = PROGRAM_PATH + _T("\\audio"); //strVoiceFilePath
	//CString m_csFileName= _T("D:\\audio");
	wchar_t s[30] = {0};
	_stprintf_s(s, _T("%d%d%d%d%d%d"), sys2.wYear, sys2.wMonth, sys2.wDay, sys2.wHour, sys2.wMinute, sys2.wSecond/*,sys2.wMilliseconds*/);
	m_csFileName.Append(s);
	m_csFileName.Append(_T(".wav"));

	m_file.Open(m_csFileName,CFile::modeCreate|CFile::modeReadWrite, &fileException);
	DWORD m_WaveHeaderSize = 38;
	DWORD m_WaveFormatSize = 18;
	m_file.SeekToBegin();
	m_file.Write("RIFF", 4);
	//unsigned int Sec=(sizeof  + m_WaveHeaderSize);
	unsigned int Sec = (sizeof m_pSaveBuffer + m_WaveHeaderSize);
	m_file.Write(&Sec, sizeof(Sec));
	m_file.Write("WAVE", 4);
	m_file.Write("fmt ", 4);
	m_file.Write(&m_WaveFormatSize, sizeof(m_WaveFormatSize));
	m_file.Write(&m_waveform.wFormatTag, sizeof(m_waveform.wFormatTag));
	m_file.Write(&m_waveform.nChannels, sizeof(m_waveform.nChannels));
	m_file.Write(&m_waveform.nSamplesPerSec, sizeof(m_waveform.nSamplesPerSec));
	m_file.Write(&m_waveform.nAvgBytesPerSec, sizeof(m_waveform.nAvgBytesPerSec));
	m_file.Write(&m_waveform.nBlockAlign, sizeof(m_waveform.nBlockAlign));
	m_file.Write(&m_waveform.wBitsPerSample, sizeof(m_waveform.wBitsPerSample));
	m_file.Write(&m_waveform.cbSize, sizeof(m_waveform.cbSize));
	m_file.Write("data", 4);
	m_file.Write(&m_dwDataLength, sizeof(m_dwDataLength));
	m_file.Write(m_pSaveBuffer, m_dwDataLength);
	m_file.Seek(m_dwDataLength, CFile::begin);
	m_file.Close();
}


void CRecordWavDlg::OnBnClickedBtnPlay()
{
	// open waveform audio for output
	m_waveform.wFormatTag = WAVE_FORMAT_PCM;
	
	// 设置不同的声音采样格式
	/* waveform.nChannels   = 1;
	waveform.nSamplesPerSec =11025;
	waveform.nAvgBytesPerSec=11025;
	waveform.nBlockAlign    =1;
	waveform.wBitsPerSample =8; */
	
	m_waveform.nChannels = 2;
	m_waveform.nSamplesPerSec = 44100;
	m_waveform.nAvgBytesPerSec = 176400;
	m_waveform.nBlockAlign = 4;
	m_waveform.wBitsPerSample = 16;
	
	if (waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_waveform, (DWORD)this->m_hWnd, NULL, CALLBACK_WINDOW))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Audio output erro"));
	}
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
	// Reallocate save buffer memory
	m_pNewBuffer = (PBYTE)realloc (m_pSaveBuffer, m_dwDataLength +((PWAVEHDR) lParam)->dwBytesRecorded);
	if (m_pNewBuffer == NULL)
	{
		waveInClose (m_hWaveIn);
		MessageBeep (MB_ICONEXCLAMATION);
		AfxMessageBox(_T("erro memory"));
		return TRUE;
	}
	
	m_pSaveBuffer = m_pNewBuffer;
	CopyMemory (m_pSaveBuffer + m_dwDataLength, ((PWAVEHDR) lParam)->lpData,
		((PWAVEHDR) lParam)->dwBytesRecorded);
	m_dwDataLength += ((PWAVEHDR) lParam)->dwBytesRecorded;
	
	if (m_bEnding)
	{
		waveInClose (m_hWaveIn);
		return TRUE;
	}
	
	waveInAddBuffer (m_hWaveIn, (PWAVEHDR) lParam, sizeof (WAVEHDR));
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWimClose(WPARAM wParam, LPARAM lParam)
{
	KillTimer(1);
	if (0 == m_dwDataLength)
	{
		return TRUE;
	}
	
	waveInUnprepareHeader (m_hWaveIn, m_pWaveHdr1, sizeof (WAVEHDR));
	waveInUnprepareHeader (m_hWaveIn, m_pWaveHdr2, sizeof (WAVEHDR));
	
	Release();
	
	if (m_dwDataLength > 0)
	{
		//enable play
		((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
		((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
		((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(TRUE);
	}
	
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomOpen(WPARAM wParam, LPARAM lParam)
{
	// Set up header
	m_pWaveHdr1->lpData = (LPSTR)m_pSaveBuffer;
	m_pWaveHdr1->dwBufferLength = m_dwDataLength;
	m_pWaveHdr1->dwBytesRecorded = 0;
	m_pWaveHdr1->dwUser = 0;
	m_pWaveHdr1->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	m_pWaveHdr1->dwLoops = 1;
	m_pWaveHdr1->lpNext = NULL;
	m_pWaveHdr1->reserved = 0;
	
	// Prepare and write
	waveOutPrepareHeader (m_hWaveOut, m_pWaveHdr1, sizeof (WAVEHDR));
	waveOutWrite (m_hWaveOut, m_pWaveHdr1, sizeof (WAVEHDR));
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(FALSE);
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomDone(WPARAM wParam, LPARAM lParam)
{
	waveOutUnprepareHeader (m_hWaveOut, m_pWaveHdr1, sizeof (WAVEHDR));
	waveOutClose (m_hWaveOut);
	return  NULL;
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomClose(WPARAM wParam, LPARAM lParam)
{
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(TRUE);
	return 0;
}


void CRecordWavDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	Release();

	if (m_pSaveBuffer)
	{
		free(m_pSaveBuffer);
		m_pSaveBuffer = nullptr;
	}

	if (m_pNewBuffer)
	{
		//free(m_pNewBuffer);
		//m_pNewBuffer = nullptr;
	}

	if (m_pWaveHdr1)
	{
		free(m_pWaveHdr1);
		m_pWaveHdr1 = nullptr;
	}
	if (m_pWaveHdr2)
	{
		free(m_pWaveHdr2);
		m_pWaveHdr2 = nullptr;
	}
}
