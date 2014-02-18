#pragma once

#include <string>


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

	bool IsRebootByDate(std::string date);
	bool IsRebootByTime(std::string time);
	void TerminateService(std::string process);
};

extern CMonitorServiceThread g_MonitorServiceThread;
