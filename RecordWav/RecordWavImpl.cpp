// 2021-06-17, Bruce

#include "stdafx.h"
#include "RecordWavImpl.h"


CRecordWavImpl::CRecordWavImpl(void)
{
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
