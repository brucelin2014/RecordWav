
// RecordWavDlg.h : 头文件
//

#pragma once

#include "RecordWavImpl.h"


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

protected:
	afx_msg LRESULT OnMmWimOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWimData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWimClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomClose(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnBnClickedBtnRecordStop();
	afx_msg void OnBnClickedBtnPlay();

	CRecordWavImpl m_RecordWavImpl;
};
