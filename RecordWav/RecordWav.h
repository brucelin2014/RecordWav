
// RecordWav.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRecordWavApp:
// �йش����ʵ�֣������ RecordWav.cpp
//

class CRecordWavApp : public CWinApp
{
public:
	CRecordWavApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRecordWavApp theApp;