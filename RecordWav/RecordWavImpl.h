// 2021-06-17, Bruce

#pragma once

// �������
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

#define BUFFER_SIZE (44100 * 16 * 2 / 8 * 5) // ¼����������
#define FRAGMENT_SIZE 1024 * 4               // ��������С
#define FRAGMENT_NUM 4                       // ����������

class CRecordWavImpl
{
public:
	CRecordWavImpl(void);
	~CRecordWavImpl(void);

public:
	void Init(CWnd* pWndParent);
	bool RecordStart(); // ��ʼ¼��
	bool RecordStop();  // ֹͣ¼��
	bool RecordPlay();  // ����¼��

	LRESULT OnMmWimOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnMmWimData(WPARAM wParam, LPARAM lParam);  // ����¼��
	LRESULT OnMmWimClose(WPARAM wParam, LPARAM lParam); // ¼�����

	LRESULT OnMmWomOpen(WPARAM wParam, LPARAM lParam);  // ��¼��
	LRESULT OnMmWomDone(WPARAM wParam, LPARAM lParam);
	LRESULT OnMmWomClose(WPARAM wParam, LPARAM lParam);

	void Release();

	HWAVEIN m_hWaveIn;       // ������Ƶ���ݸ�ʽWave_audio���ݸ�ʽ
	WAVEFORMATEX m_waveform;
	HWAVEOUT m_hWaveOut;     // �򿪻ط��豸����
	WAVEHDR* m_pWaveHdr1;
	WAVEHDR* m_pWaveHdr2;

	PBYTE m_pBuffer1;
	PBYTE m_pBuffer2;
	PBYTE m_pSaveBuffer;
	PBYTE m_pNewBuffer;
	int m_dwDataLength;
	bool m_bEnding;

private:
	CWnd* m_pWndParent;
};

