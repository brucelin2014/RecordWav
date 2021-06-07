
// RecordWavDlg.h : 头文件
//

#pragma once

// 声音相关
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

#define BUFFER_SIZE (44100*16*2/8*5) // 录制声音长度
#define FRAGMENT_SIZE 1024*4         // 缓存区大小
#define FRAGMENT_NUM 4               // 缓存区个数


// CRecordWavDlg 对话框
class CRecordWavDlg : public CDialogEx
{
// 构造
public:
	CRecordWavDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_RECORDWAV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnRecordStart();

	HWAVEIN hWaveIn;       // 波形音频数据格式Wave_audio数据格式
	WAVEFORMATEX waveform;
	HWAVEOUT hWaveOut;     // 打开回放设备函数
	WAVEHDR *pWaveHdr1;
	WAVEHDR *pWaveHdr2;

	PBYTE pBuffer1;
	PBYTE pBuffer2;
	PBYTE pSaveBuffer;
	PBYTE pNewBuffer;
	int dwDataLength;
	bool bEnding;
	afx_msg void OnBnClickedBtnRecordStop();
	afx_msg void OnBnClickedBtnPlay();
protected:
	afx_msg LRESULT OnMmWimOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWimData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWimClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomClose(WPARAM wParam, LPARAM lParam);
};
