// 2021-06-17, Bruce

#pragma once

// 声音相关
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

#define BUFFER_SIZE (44100 * 16 * 2 / 8 * 5) // 录制声音长度
#define FRAGMENT_SIZE 1024 * 4               // 缓存区大小
#define FRAGMENT_NUM 4                       // 缓存区个数

class CRecordWavImpl
{
public:
	CRecordWavImpl(void);
	~CRecordWavImpl(void);

private:
	void Release();

	HWAVEIN m_hWaveIn;       // 波形音频数据格式Wave_audio数据格式
	WAVEFORMATEX m_waveform;
	HWAVEOUT m_hWaveOut;     // 打开回放设备函数
	WAVEHDR* m_pWaveHdr1;
	WAVEHDR* m_pWaveHdr2;

	PBYTE m_pBuffer1;
	PBYTE m_pBuffer2;
	PBYTE m_pSaveBuffer;
	PBYTE m_pNewBuffer;
	int m_dwDataLength;
	bool m_bEnding;
};

