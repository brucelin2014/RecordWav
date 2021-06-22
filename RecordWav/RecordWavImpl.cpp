// 2021-06-17, Bruce

#include "stdafx.h"
#include "RecordWavImpl.h"


CRecordWavImpl::CRecordWavImpl(void)
{
	m_pWndParent = nullptr;

	m_pBuffer1 = nullptr;
	m_pBuffer2 = nullptr;
	m_pSaveBuffer = nullptr;
	m_pNewBuffer = nullptr;
	m_pWaveHdr1 = nullptr;
	m_pWaveHdr2 = nullptr;
}


CRecordWavImpl::~CRecordWavImpl(void)
{
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

void CRecordWavImpl::Release()
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

void CRecordWavImpl::Init(CWnd* pWndParent)
{
	m_pWndParent = pWndParent;

	m_dwDataLength = FRAGMENT_SIZE;

	//allocate memory for wave header
	m_pWaveHdr1 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	m_pWaveHdr2 = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));

	//allocate memory for save buffer
	m_pSaveBuffer = reinterpret_cast<PBYTE>(malloc(1));
}

bool CRecordWavImpl::RecordStart()
{
	Release();

	// allocate buffer memory
	m_pBuffer1 = (PBYTE)malloc(m_dwDataLength);
	m_pBuffer2 = (PBYTE)malloc(m_dwDataLength);
	if (!m_pBuffer1 || !m_pBuffer2)
	{
		Release();
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Memory error!"));
		return false;
	}

	// open waveform audio for input
	m_waveform.wFormatTag = WAVE_FORMAT_PCM;
	m_waveform.nChannels = 2;
	m_waveform.nSamplesPerSec = 44100;
	m_waveform.nAvgBytesPerSec = 176400;
	m_waveform.nBlockAlign = 4;
	m_waveform.wBitsPerSample = 16;
	m_waveform.cbSize = 0;

	if (waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveform, (DWORD)m_pWndParent->m_hWnd, NULL, CALLBACK_WINDOW))
	{
		Release();
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Audio can not be open!"));
		return false;
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

	m_pWaveHdr2->lpData = (LPSTR)m_pBuffer2;
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
	return true;
}

bool CRecordWavImpl::RecordStop()
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
			return false;
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

	return true;
}

bool CRecordWavImpl::RecordPlay()
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
	
	if (waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_waveform, (DWORD)m_pWndParent->m_hWnd, NULL, CALLBACK_WINDOW))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Audio output erro"));
		return false;
	}
	return true;
}

LRESULT CRecordWavImpl::OnMmWimOpen(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

LRESULT CRecordWavImpl::OnMmWimData(WPARAM wParam, LPARAM lParam)
{
	// Reallocate save buffer memory
	m_pNewBuffer = (PBYTE)realloc (m_pSaveBuffer, m_dwDataLength +((PWAVEHDR) lParam)->dwBytesRecorded);
	if (m_pNewBuffer == NULL)
	{
		waveInClose (m_hWaveIn);
		MessageBeep (MB_ICONEXCLAMATION);
		AfxMessageBox(_T("erro memory"));
		return 0;
	}

	m_pSaveBuffer = m_pNewBuffer;
	CopyMemory (m_pSaveBuffer + m_dwDataLength, ((PWAVEHDR) lParam)->lpData,
		((PWAVEHDR) lParam)->dwBytesRecorded);
	m_dwDataLength += ((PWAVEHDR) lParam)->dwBytesRecorded;

	if (m_bEnding)
	{
		waveInClose (m_hWaveIn);
		return 0;
	}

	waveInAddBuffer (m_hWaveIn, (PWAVEHDR) lParam, sizeof (WAVEHDR));
	return 1;
}

LRESULT CRecordWavImpl::OnMmWimClose(WPARAM wParam, LPARAM lParam)
{
	if (0 == m_dwDataLength)
	{
		return 0;
	}

	waveInUnprepareHeader (m_hWaveIn, m_pWaveHdr1, sizeof (WAVEHDR));
	waveInUnprepareHeader (m_hWaveIn, m_pWaveHdr2, sizeof (WAVEHDR));

	Release();

	if (m_dwDataLength > 0)
	{
		// enable play
		return 1;
	}
	return 2;
}

LRESULT CRecordWavImpl::OnMmWomOpen(WPARAM wParam, LPARAM lParam)
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
	return 1;
}

LRESULT CRecordWavImpl::OnMmWomDone(WPARAM wParam, LPARAM lParam)
{
	waveOutUnprepareHeader (m_hWaveOut, m_pWaveHdr1, sizeof (WAVEHDR));
	waveOutClose (m_hWaveOut);
	return 1;
}

LRESULT CRecordWavImpl::OnMmWomClose(WPARAM wParam, LPARAM lParam)
{
	return 1;
}