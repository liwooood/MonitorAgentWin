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
		

		// ���û�����ӣ��ͽ�������
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
			// ���ӳɹ�

			if (pThis->m_Conn.HeartBeat())
			{
				// ���������ɹ�
				Sleep(sConfigManager::instance().m_nServiceCheck * 1000);
				
				continue;
			}
			else
			{
				// ��������ʧ��
				gFileLog::instance().Log("����������ʧ�ܣ�����������");

				// ���´��벻ʵ�֣�Ҳ����ν����������ʧ�ܵĴ����������

				//pThis->m_Conn.Close();

				// ɱ������
				g_MonitorProcessThread.TerminateAllService();

				// ������������
				g_MonitorProcessThread.StartService(sConfigManager::instance().m_vService[0].m_sProcess);

				// �ȴ�������ʼ��
				Sleep(sConfigManager::instance().m_nServiceInit * 1000);

				continue;
			}
		}
		else
		{
			// ����ʧ��
			gFileLog::instance().Log("���ӽ�������ʧ�ܣ�����������");

			// ɱ������
			g_MonitorProcessThread.TerminateAllService();

			// ������������
			g_MonitorProcessThread.StartService(sConfigManager::instance().m_vService[0].m_sProcess);

			// �ȴ�������ʼ��
			Sleep(sConfigManager::instance().m_nServiceInit * 1000);

			continue;
		}

	} // end while

	return 0;
}
