#pragma once

#include <string>

/*
ԭ��
1.���̲����ھ��������ȴ�����������ɻ������ó�ʱʱ��
2.���̴��ھͼ�����ѯ
*/

class CMonitorProcessThread
{
public:
	CMonitorProcessThread(void);
	~CMonitorProcessThread(void);
	void start();
	void stop();

private:
	static unsigned WINAPI ThreadFunc(void * pThis);
	HANDLE m_hThread;
	bool m_bRunning;

	bool TcpHeartBeat(std::string server);
	bool SslHeartBeat(std::string server);

public:
	bool IsProcessExist(DWORD dwProcessId);
	bool IsProcessExist(std::string sProcessName);

	void TerminateService(std::string process);
	void TerminateAllService();

	
	int StartService(std::string sExe);
};

extern CMonitorProcessThread g_MonitorProcessThread;