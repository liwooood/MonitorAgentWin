#pragma once

#include "TCPClientSync.h"

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

	CTCPClientSync m_Conn;
};

extern CMonitorServiceThread g_MonitorServiceThread;
