#include "StdAfx.h"

#include "MonitorServiceThread.h"
#include "config/ConfigManager.h"
#include "network/TCPClientSync.h"
#include "MonitorProcessThread.h"
#include "output/FileLog.h"


CMonitorServiceThread g_MonitorServiceThread;


CMonitorServiceThread::CMonitorServiceThread(void)
{
}


CMonitorServiceThread::~CMonitorServiceThread(void)
{
}

void CMonitorServiceThread::start()
{

	m_hThread = NULL;
	m_bRunning = true;

	m_hThread = (HANDLE) _beginthreadex(NULL, 0, &(CMonitorServiceThread::ThreadFunc), this, 0, NULL);

	if (m_hThread == NULL)
	{
		m_bRunning = false;
	}
}

void CMonitorServiceThread::stop()
{
	

	m_bRunning = false;

	if (m_hThread != NULL)
	{
		
		//TerminateThread(m_hThread, 0);
		//_endthreadex(m_hThread);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	
	
}

unsigned WINAPI CMonitorServiceThread::ThreadFunc(void * pParam)
{
	CMonitorServiceThread * pThis = (CMonitorServiceThread*) pParam;
	bool bConnect = false;

	while(pThis->m_bRunning)
	{
		

		// 如果没有连接，就建立连接
		if (!pThis->m_Conn.IsConnected())
		{
			std::vector<std::string> kv;
			boost::split(kv, sConfigManager::instance().m_vService[0].m_sServer, boost::is_any_of(":"));

			std::string ip = kv[0];
			int port = boost::lexical_cast<int>(kv[1]);

			bConnect = pThis->m_Conn.Connect(ip, port);
		}

		if (bConnect)
		{
			// 连接成功

			if (pThis->m_Conn.HeartBeat())
			{
				// 发心跳包成功
				Sleep(sConfigManager::instance().m_nServiceCheck * 1000);
				
				continue;
			}
			else
			{
				// 发心跳包失败
				gFileLog::instance().Log("发送心跳包失败，将启动服务");

				// 以下代码不实现，也无所谓，会由连接失败的代码段来处理

				//pThis->m_Conn.Close();

				// 杀掉进程
				g_MonitorProcessThread.TerminateAllService();

				// 启动交易网关
				g_MonitorProcessThread.StartService(sConfigManager::instance().m_vService[0].m_sProcess);

				// 等待启动初始化
				Sleep(sConfigManager::instance().m_nServiceInit * 1000);

				continue;
			}
		}
		else
		{
			// 连接失败
			gFileLog::instance().Log("连接交易网关失败，将启动服务");

			// 杀掉进程
			g_MonitorProcessThread.TerminateAllService();

			// 启动交易网关
			g_MonitorProcessThread.StartService(sConfigManager::instance().m_vService[0].m_sProcess);

			// 等待启动初始化
			Sleep(sConfigManager::instance().m_nServiceInit * 1000);

			continue;
		}

	} // end while

	return 0;
}
