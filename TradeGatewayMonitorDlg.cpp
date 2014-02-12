
// TradeGatewayMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TradeGatewayMonitor.h"
#include "TradeGatewayMonitorDlg.h"

#include "MonitorProcessThread.h"
#include "config/ConfigManager.h"
#include "resource.h"

#include "network/TCPClientSync.h"
#include "network/TCPClientASync.h"

#include "MonitorServiceThread.h"
#include "output/FileLog.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTradeGatewayMonitorDlg 对话框




CTradeGatewayMonitorDlg::CTradeGatewayMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTradeGatewayMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTradeGatewayMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTradeGatewayMonitorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EXIT, &CTradeGatewayMonitorDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_TEST_SERVICE, &CTradeGatewayMonitorDlg::OnBnClickedTestService)
	ON_BN_CLICKED(IDC_OPEN_LOG, &CTradeGatewayMonitorDlg::OnBnClickedOpenLog)
	ON_BN_CLICKED(IDC_TEST_SERVICE2, &CTradeGatewayMonitorDlg::OnBnClickedTestService2)
	ON_BN_CLICKED(IDC_BUTTON2, &CTradeGatewayMonitorDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CTradeGatewayMonitorDlg 消息处理程序

BOOL CTradeGatewayMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	Init();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTradeGatewayMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTradeGatewayMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTradeGatewayMonitorDlg::OnBnClickedExit()
{
	int nResult = MessageBox("退出监控程序，将关闭所有被监控进程，确定退出吗？", "警告框", MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2);
	if (nResult != IDOK)
		return;

	

	g_MonitorProcessThread.stop();

	g_MonitorServiceThread.stop();

	//SendMessage(WM_CLOSE);

	EndDialog(0);
}

void CTradeGatewayMonitorDlg::OnCancel()
{
}

void CTradeGatewayMonitorDlg::Init()
{
	CService& service = sConfigManager::instance().m_vService[0];

	CString serviceName = "服务名字：";
	serviceName += service.m_sServiceName.c_str();
	this->GetDlgItem(IDC_SERVICE_NAME)->SetWindowText(serviceName);

	CString processFile = "进程路径：";
	processFile += service.m_sProcess.c_str();
	this->GetDlgItem(IDC_PROCSS_FILE)->SetWindowText(processFile);

	gFileLog::instance().Log(LOG_LEVEL_INFO, "监控代理启动");

	// 启动进程监控
	g_MonitorProcessThread.start();	

	// 启动服务监控
	//g_MonitorServiceThread.start();
}


void CTradeGatewayMonitorDlg::OnBnClickedOpenLog()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, "open", sConfigManager::instance().m_sPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void CTradeGatewayMonitorDlg::OnBnClickedTestService()
{
	// TODO: 在此添加控件通知处理程序代码
	CTCPClientSync conn;

	conn.Connect("127.0.0.1", 6001);

	for (int i=0; i<1000; i++)
	{
		TRACE("==================i=%d===============\n", i);
		if(!conn.HeartBeat())
			break;
	}

	conn.Close();
}


void CTradeGatewayMonitorDlg::OnBnClickedTestService2()
{
	/*
CTCPClientASync  * pTcpClientAsync = new CTCPClientASync();

pTcpClientAsync->init();

	pTcpClientAsync->Connect("127.0.0.1", 5001);

	for (int i=0; i<300; i++)
	{
		
		pTcpClientAsync->HeartBeat();
	}

	//conn.Close();
	*/
}


void CTradeGatewayMonitorDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
}
