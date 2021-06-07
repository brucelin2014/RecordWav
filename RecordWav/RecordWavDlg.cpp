
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

	dwDataLength = FRAGMENT_SIZE;

	//allocate memory for wave header
	pWaveHdr1 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	pWaveHdr2 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	//allocate memory for save buffer
	pSaveBuffer = reinterpret_cast<PBYTE>(malloc(1));

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
	// allocate buffer memory
	pBuffer1 = (PBYTE)malloc(dwDataLength);
	pBuffer2 = (PBYTE)malloc(dwDataLength);
	if (!pBuffer1 || !pBuffer2)
	{
		if (pBuffer1)
			free(pBuffer1);
		if (pBuffer2)
			free(pBuffer2);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Memory erro!"));
		return;
	}

	// open waveform audio for input
	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nChannels = 2;
	waveform.nSamplesPerSec = 44100;
	waveform.nAvgBytesPerSec = 176400;
	waveform.nBlockAlign = 4;
	waveform.wBitsPerSample = 16;
	waveform.cbSize = 0;
	
	if (waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD)this->m_hWnd, NULL, CALLBACK_WINDOW))
	{
		free(pBuffer1);
		free(pBuffer2);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Audio can not be open!"));
	}

	pWaveHdr1->lpData = (LPSTR)pBuffer1;
	pWaveHdr1->dwBufferLength = dwDataLength;
	pWaveHdr1->dwBytesRecorded = 0;
	pWaveHdr1->dwUser = 0;
	pWaveHdr1->dwFlags = 0;
	pWaveHdr1->dwLoops = 1;
	pWaveHdr1->lpNext = NULL;
	pWaveHdr1->reserved = 0;
	
	waveInPrepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
	
	pWaveHdr2->lpData = (LPSTR)pBuffer2;//
	pWaveHdr2->dwBufferLength = dwDataLength;
	pWaveHdr2->dwBytesRecorded = 0;
	pWaveHdr2->dwUser = 0;
	pWaveHdr2->dwFlags = 0;
	pWaveHdr2->dwLoops = 1;
	pWaveHdr2->lpNext = NULL;
	pWaveHdr2->reserved = 0;
	
	waveInPrepareHeader(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));
	
	pSaveBuffer = (PBYTE)realloc (pSaveBuffer, 1);
	
	// Add the buffers
	waveInAddBuffer (hWaveIn, pWaveHdr1, sizeof (WAVEHDR));
	waveInAddBuffer (hWaveIn, pWaveHdr2, sizeof (WAVEHDR));
	
	// Begin sampling
	bEnding = FALSE;
	dwDataLength = 0;
	waveInStart (hWaveIn);
}


void CRecordWavDlg::OnBnClickedBtnRecordStop()
{
	bEnding = TRUE;
	// 停止录音
	waveInReset(hWaveIn);
	
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
	unsigned int Sec = (sizeof pSaveBuffer + m_WaveHeaderSize);
	m_file.Write(&Sec, sizeof(Sec));
	m_file.Write("WAVE", 4);
	m_file.Write("fmt ", 4);
	m_file.Write(&m_WaveFormatSize, sizeof(m_WaveFormatSize));
	m_file.Write(&waveform.wFormatTag, sizeof(waveform.wFormatTag));
	m_file.Write(&waveform.nChannels, sizeof(waveform.nChannels));
	m_file.Write(&waveform.nSamplesPerSec, sizeof(waveform.nSamplesPerSec));
	m_file.Write(&waveform.nAvgBytesPerSec, sizeof(waveform.nAvgBytesPerSec));
	m_file.Write(&waveform.nBlockAlign, sizeof(waveform.nBlockAlign));
	m_file.Write(&waveform.wBitsPerSample, sizeof(waveform.wBitsPerSample));
	m_file.Write(&waveform.cbSize, sizeof(waveform.cbSize));
	m_file.Write("data", 4);
	m_file.Write(&dwDataLength, sizeof(dwDataLength));
	m_file.Write(pSaveBuffer, dwDataLength);
	m_file.Seek(dwDataLength, CFile::begin);
	m_file.Close();
}


void CRecordWavDlg::OnBnClickedBtnPlay()
{
	// open waveform audio for output
	waveform.wFormatTag = WAVE_FORMAT_PCM;
	
	// 设置不同的声音采样格式
	/* waveform.nChannels   = 1;
	waveform.nSamplesPerSec =11025;
	waveform.nAvgBytesPerSec=11025;
	waveform.nBlockAlign    =1;
	waveform.wBitsPerSample =8; */
	
	waveform.nChannels = 2;
	waveform.nSamplesPerSec = 44100;
	waveform.nAvgBytesPerSec = 176400;
	waveform.nBlockAlign = 4;
	waveform.wBitsPerSample = 16;
	
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD)this->m_hWnd, NULL, CALLBACK_WINDOW))
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
	pNewBuffer = (PBYTE)realloc (pSaveBuffer, dwDataLength +((PWAVEHDR) lParam)->dwBytesRecorded);
	if (pNewBuffer == NULL)
	{
		waveInClose (hWaveIn);
		MessageBeep (MB_ICONEXCLAMATION);
		AfxMessageBox(_T("erro memory"));
		return TRUE;
	}
	
	pSaveBuffer = pNewBuffer;
	CopyMemory (pSaveBuffer + dwDataLength, ((PWAVEHDR) lParam)->lpData,
		((PWAVEHDR) lParam)->dwBytesRecorded);
	dwDataLength += ((PWAVEHDR) lParam)->dwBytesRecorded;
	
	if (bEnding)
	{
		waveInClose (hWaveIn);
		return TRUE;
	}
	
	waveInAddBuffer (hWaveIn, (PWAVEHDR) lParam, sizeof (WAVEHDR));
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWimClose(WPARAM wParam, LPARAM lParam)
{
	KillTimer(1);
	if (0 == dwDataLength)
	{
		return TRUE;
	}
	
	waveInUnprepareHeader (hWaveIn, pWaveHdr1, sizeof (WAVEHDR));
	waveInUnprepareHeader (hWaveIn, pWaveHdr2, sizeof (WAVEHDR));
	
	free (pBuffer1);
	free (pBuffer2);
	
	if (dwDataLength > 0)
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
	pWaveHdr1->lpData = (LPSTR)pSaveBuffer;
	pWaveHdr1->dwBufferLength = dwDataLength;
	pWaveHdr1->dwBytesRecorded = 0;
	pWaveHdr1->dwUser = 0;
	pWaveHdr1->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	pWaveHdr1->dwLoops = 1;
	pWaveHdr1->lpNext = NULL;
	pWaveHdr1->reserved = 0;
	
	// Prepare and write
	waveOutPrepareHeader (hWaveOut, pWaveHdr1, sizeof (WAVEHDR));
	waveOutWrite (hWaveOut, pWaveHdr1, sizeof (WAVEHDR));
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_START)))->EnableWindow(TRUE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_RECORD_STOP)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BTN_PLAY)))->EnableWindow(FALSE);
	return 0;
}


afx_msg LRESULT CRecordWavDlg::OnMmWomDone(WPARAM wParam, LPARAM lParam)
{
	waveOutUnprepareHeader (hWaveOut, pWaveHdr1, sizeof (WAVEHDR));
	waveOutClose (hWaveOut);
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
