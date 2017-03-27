
// VideoMFCDlg.h : ͷ�ļ�
//

#pragma once
#include <opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "CvvImage.h"
#include "detetc_recog.h"
#include <conio.h>

// CVideoMFCDlg �Ի���
class CVideoMFCDlg : public CDialogEx
{
// ����
public:
	CVideoMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VIDEOMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	IplImage* image;
	//	CvCapture* capture;
	afx_msg void OnBnClickedButton1();
private:
	//	void OnTimer(UINT nIDEvent);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton3();
	//	CStatic m_showImage2;
	afx_msg void OnStnClickedShowimage();
	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnStnClickedNo3();
	afx_msg void OnStnClickedNo7();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnStnClickedStatic01();
};
