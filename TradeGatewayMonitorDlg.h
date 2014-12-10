
// TradeGatewayMonitorDlg.h : 头文件
//

#pragma once


// CTradeGatewayMonitorDlg 对话框
class CTradeGatewayMonitorDlg : public CDialog
{
// 构造
public:
	CTradeGatewayMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TRADEGATEWAYMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
