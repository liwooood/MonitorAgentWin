#include "StdAfx.h"

#include <psapi.h>
#include <TlHelp32.h>
#include <process.h>

#include "MonitorServiceThread.h"
#include "ConfigManager.h"
#include "TCPClientSync.h"
#include "MonitorProcessThread.h"
#include "FileLog.h"


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
		
		std::vector<CService>& m_vService = sConfigManager::instance().m_vService;

		for (size_t i=0; i<m_vService.size(); i++)
		{
			CService& service = m_vService[i];

			if (pThis->IsRebootByDate(service.m_sRebootDate))
			{
				if (pThis->IsRebootByTime(service.m_sRebootTime))
				{
					// 根据配置文件配置的时间点来重新初始化服务
					gFileLog::instance().Log(LOG_LEVEL_ERROR, "定时重启服务");
					pThis->TerminateService(service.m_sProcess);
				}
			}
		}

		Sleep(1000);

	} // end while
	
	return 0;
}

/*
判断当天日期和传入的参数是否相等
*/
bool CMonitorServiceThread::IsRebootByDate(std::string date)
{
	bool bRet = false;

	if (boost::algorithm::iequals(date, "none"))
	{
		bRet = false;
		return bRet;
	}

	if (boost::algorithm::iequals(date, "all"))
	{
		bRet = true;
		return bRet;
	}

	boost::gregorian::date d = boost::gregorian::day_clock::local_day();
	int dayOfWeek = d.day_of_week(); //星期天从0开始

	int today = boost::lexical_cast<int>(date);
	if (today == dayOfWeek)
		return true;
	

	return false;
}

/*
判断当前时间是否和参数传入的时间time相等
*/
bool CMonitorServiceThread::IsRebootByTime(std::string time)
{
	bool bRet = false;

	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	std::string nowTime = boost::posix_time::to_simple_string(now.time_of_day());
	//TRACE("now time %s\n", nowTime.c_str());

	if (nowTime == time)
	{
		return true;
	}

	return bRet;
}

void CMonitorServiceThread::TerminateService(std::string process)
{

	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != NULL)
	{
		PROCESSENTRY32 pe;
		::ZeroMemory(&pe, sizeof(pe));
		pe.dwSize = sizeof(pe);



		if (::Process32First(hSnapshot, &pe))
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

			TCHAR szModName[MAX_PATH];
			memset(szModName, 0, MAX_PATH);
			GetModuleFileNameEx(hProcess, NULL, szModName, MAX_PATH);
			std::string filename = szModName;
			//TRACE("process path first: %s\n", filename.c_str());
			

			if ( boost::algorithm::iequals(process, filename) )
			{
				TerminateProcess(hProcess, 0);
			}

			while (::Process32Next(hSnapshot, &pe))
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

				TCHAR szModName[MAX_PATH];
				memset(szModName, 0, MAX_PATH);
				GetModuleFileNameEx(hProcess, NULL, szModName, MAX_PATH);
				std::string filename = szModName;
			
				//TRACE("process path next: %s\n", filename.c_str());

				if ( boost::algorithm::iequals(process, filename) )
				{
					TerminateProcess(hProcess, 0);
				}
			}//end while

		}//if (::Process32First(hSnapshot, &pe))
	}//if (hSnapshot != NULL)

	CloseHandle(hSnapshot);
	hSnapshot = INVALID_HANDLE_VALUE;

}
