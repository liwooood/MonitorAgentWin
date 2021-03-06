
// TradeGatewayMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TradeGatewayMonitor.h"
#include "TradeGatewayMonitorDlg.h"

#include "MonitorProcessThread.h"
#include "ConfigManager.h"
#include "resource.h"

#include "TCPClientSync.h"
#include "SSLClientSync.h"

#include "MonitorServiceThread.h"
#include "FileLog.h"
#include "TCPClientASync.h"



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
	
	ON_BN_CLICKED(IDC_OPEN_LOG, &CTradeGatewayMonitorDlg::OnBnClickedOpenLog)
	
	
	ON_BN_CLICKED(IDC_TEST_SSL_PB, &CTradeGatewayMonitorDlg::OnBnClickedTestSslPb)
	ON_BN_CLICKED(IDC_TEST_TCP, &CTradeGatewayMonitorDlg::OnBnClickedTestTcp)
	ON_BN_CLICKED(IDC_TEST_TCP_ASYNC, &CTradeGatewayMonitorDlg::OnBnClickedTestTcpAsync)
	ON_BN_CLICKED(IDC_TEST_SSL_ASYNC, &CTradeGatewayMonitorDlg::OnBnClickedTestSslAsync)
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


void CTradeGatewayMonitorDlg::Init()
{
	CService& service = gConfigManager::instance().m_vService[0];

	CString serviceName = "服务名字：";
	serviceName += service.m_sServiceName.c_str();
	this->GetDlgItem(IDC_SERVICE_NAME)->SetWindowText(serviceName);

	CString processFile = "进程路径：";
	processFile += service.m_sProcess.c_str();
	this->GetDlgItem(IDC_PROCSS_FILE)->SetWindowText(processFile);

	gFileLog::instance().Log(LOG_LEVEL_INFO, "监控代理启动");

	// 定时重启
	g_MonitorServiceThread.start();

	if (gConfigManager::instance().runMode)
	{
		// 启动进程监控
		g_MonitorProcessThread.start();	
	}

	std::string caption = "中软万维监控代理 版本1.1";
	SetWindowText(caption.c_str());

	// 启动ioservice run
	tcpAsync.init();
}


void CTradeGatewayMonitorDlg::OnBnClickedOpenLog()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, "open", gConfigManager::instance().m_sPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}





void CTradeGatewayMonitorDlg::OnBnClickedTestSslPb()
{
	// TODO: 在此添加控件通知处理程序代码
	boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
		SSLClientSync conn(ctx);

		

	for (int i=0; i < 1; i++)
	{
		TRACE("==================i=%d===============\n", i);
		

	
		

		conn.Connect("127.0.0.1", 5000);
		conn.HeartBeat();
		conn.Close();
	}
}


void CTradeGatewayMonitorDlg::OnBnClickedTestTcp()
{
	// TODO: 在此添加控件通知处理程序代码
		CTCPClientSync conn;

	for (int i=0; i < 1; i++)
	{
		TRACE("==================i=%d===============\n", i);
		

	
		

		conn.Connect("127.0.0.1", 5001);
		conn.HeartBeat();
		conn.Close();
	}
}


void CTradeGatewayMonitorDlg::OnBnClickedTestTcpAsync()
{
	// TODO: 在此添加控件通知处理程序代码
	

	tcpAsync.Connect("127.0.0.1", 5001);
	tcpAsync.Read();

	for (int i=0; i<5; i++)
	{

		tcpAsync.HeartBeat();
		Sleep(3000);
	}
}


void CTradeGatewayMonitorDlg::OnBnClickedTestSslAsync()
{
	// TODO: 在此添加控件通知处理程序代码
}
