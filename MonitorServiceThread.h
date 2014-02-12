#pragma once



/*
³¬Ê±±¨´í
*/

class CMonitorServiceThread
{
public:
	CMonitorServiceThread(void);
	~CMonitorServiceThread(void);
	void start();
	void stop();

private:
	static unsigned WINAPI ThreadFunc(void * pThis);
	HANDLE m_hThread;
	bool m_bRunning;

	
};

extern CMonitorServiceThread g_MonitorServiceThread;
