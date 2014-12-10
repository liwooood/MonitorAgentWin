#pragma once

#include <string>

/*
原理
1.进程不存在就启动，等待进程启动完成或者设置超时时间
2.进程存在就继续轮询
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