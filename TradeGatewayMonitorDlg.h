
// TradeGatewayMonitorDlg.h : ͷ�ļ�
//

#pragma once


// CTradeGatewayMonitorDlg �Ի���
class CTradeGatewayMonitorDlg : public CDialog
{
// ����
public:
	CTradeGatewayMonitorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRADEGATEWAYMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();
	

	void Init();
	
	afx_msg void OnBnClickedOpenLog();
	
	
	afx_msg void OnBnClickedTestSslPb();
	afx_msg void OnBnClickedTestTcp();
};
