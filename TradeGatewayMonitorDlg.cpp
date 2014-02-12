
// TradeGatewayMonitorDlg.cpp : ʵ���ļ�
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


// CTradeGatewayMonitorDlg �Ի���




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


// CTradeGatewayMonitorDlg ��Ϣ�������

BOOL CTradeGatewayMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	Init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTradeGatewayMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTradeGatewayMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTradeGatewayMonitorDlg::OnBnClickedExit()
{
	int nResult = MessageBox("�˳���س��򣬽��ر����б���ؽ��̣�ȷ���˳���", "�����", MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2);
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

	CString serviceName = "�������֣�";
	serviceName += service.m_sServiceName.c_str();
	this->GetDlgItem(IDC_SERVICE_NAME)->SetWindowText(serviceName);

	CString processFile = "����·����";
	processFile += service.m_sProcess.c_str();
	this->GetDlgItem(IDC_PROCSS_FILE)->SetWindowText(processFile);

	gFileLog::instance().Log(LOG_LEVEL_INFO, "��ش�������");

	// �������̼��
	g_MonitorProcessThread.start();	

	// ����������
	//g_MonitorServiceThread.start();
}


void CTradeGatewayMonitorDlg::OnBnClickedOpenLog()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShellExecute(NULL, "open", sConfigManager::instance().m_sPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void CTradeGatewayMonitorDlg::OnBnClickedTestService()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
