#include "StdAfx.h"
#include "MonitorProcessThread.h"



#include <psapi.h>
#include <TlHelp32.h>
#include <process.h>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include "config/ConfigManager.h"
#include "output/FileLog.h"


#pragma comment(lib, "Psapi.lib")

CMonitorProcessThread g_MonitorProcessThread;


CMonitorProcessThread::CMonitorProcessThread(void)
{

}

CMonitorProcessThread::~CMonitorProcessThread(void)
{
	
}

void CMonitorProcessThread::start()
{
	TerminateAllService();

	m_hThread = NULL;
	m_bRunning = true;

	m_hThread = (HANDLE) _beginthreadex(NULL, 0, &(CMonitorProcessThread::ThreadFunc), this, 0, NULL);

	if (m_hThread == NULL)
	{
		m_bRunning = false;
	}
}

void CMonitorProcessThread::stop()
{
	

	m_bRunning = false;

	if (m_hThread != NULL)
	{
		
		//TerminateThread(m_hThread, 0);
		//_endthreadex(m_hThread);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	TerminateAllService();
	
}

unsigned WINAPI CMonitorProcessThread::ThreadFunc(void * pParam)
{

	CMonitorProcessThread * pThis = (CMonitorProcessThread*) pParam;
	while(pThis->m_bRunning)
	{
		std::vector<CService>& m_vService = sConfigManager::instance().m_vService;

		for (size_t i=0; i<m_vService.size(); i++)
		{
			CService& service = m_vService[i];

			if (pThis->IsProcessExist(service.m_sProcess))
			{
				//TRACE("进程存在\n");
				if (pThis->IsRebootByDate(service.m_sRebootDate))
				{
					if (pThis->IsRebootByTime(service.m_sRebootTime))
					{
						// 根据配置文件配置的时间点来重新初始化服务
						gFileLog::instance().Log("定时重启服务");
						pThis->TerminateService(service.m_sProcess);
					}
				}
			}
			else
			{
				//TRACE("进程不存在\n");
				//pThis->StartService(service.m_sProcess);
				// 由服务监控来管理

			} // end if
		}//end for

		Sleep(sConfigManager::instance().m_nProcessCheck * 1000);
	} // end while

	

	return 0;
}

int CMonitorProcessThread::StartService(std::string sExe)
{
	
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
				

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
				
	return CreateProcess(sExe.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	
	/*
	SHELLEXECUTEINFO sei; 
	memset(&sei, 0, sizeof(SHELLEXECUTEINFO)); 
	
	sei.cbSize = sizeof(SHELLEXECUTEINFO); 
	sei.fMask = SEE_MASK_NOCLOSEPROCESS; 
	sei.lpVerb = _T("open"); 
	sei.lpFile = sExe.c_str(); 
	sei.lpDirectory = NULL; 
	sei.lpParameters = NULL;
	sei.nShow = SW_SHOW; 

	ShellExecuteEx(&sei); 
	//ShellExecute(NULL, "Open", sOCRExe.c_str(), sParam.c_str(), sOCRPath.c_str(), SW_HIDE);

	WaitForSingleObject(sei.hProcess, INFINITE); 
	CloseHandle(sei.hProcess);  
	*/
}

bool CMonitorProcessThread::IsProcessExist(DWORD dwProcessId)
{

	bool bFound = false;

	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != NULL)
	{
		PROCESSENTRY32 pe;
		::ZeroMemory(&pe, sizeof(pe));
		pe.dwSize = sizeof(pe);

		if (::Process32First(hSnapshot, &pe))
		{
			bFound = pe.th32ProcessID == dwProcessId;
			while (!bFound && ::Process32Next(hSnapshot, &pe))
			{
				//TRACE("%s\n",pe.szExeFile);
				bFound = pe.th32ProcessID == dwProcessId;
			}
		}
	}

	CloseHandle(hSnapshot);
	hSnapshot = INVALID_HANDLE_VALUE;

	return bFound;
}

bool CMonitorProcessThread::IsProcessExist(std::string sProcessName)
{

	bool bFound = false;

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
			

			if ( boost::algorithm::iequals(sProcessName, filename) )
				bFound = true;
			else
				bFound = false;

			while (!bFound && ::Process32Next(hSnapshot, &pe))
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

				TCHAR szModName[MAX_PATH];
				memset(szModName, 0, MAX_PATH);
				GetModuleFileNameEx(hProcess, NULL, szModName, MAX_PATH);
				std::string filename = szModName;
				//TRACE("process path first: %s\n", filename.c_str());
			

				if ( boost::algorithm::iequals(sProcessName, filename) )
					bFound = true;
				else
					bFound = false;
			}
		}
	}

	CloseHandle(hSnapshot);
	hSnapshot = INVALID_HANDLE_VALUE;

	return bFound;
}

void CMonitorProcessThread::TerminateAllService()
{
	std::vector<CService>& m_vService = sConfigManager::instance().m_vService;

	for (size_t i=0; i<m_vService.size(); i++)
	{
			CService& service = m_vService[i];

			TerminateService(service.m_sProcess);
	}
}

void CMonitorProcessThread::TerminateService(std::string process)
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

/*
判断当天日期和传入的参数是否相等
*/
bool CMonitorProcessThread::IsRebootByDate(std::string date)
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
bool CMonitorProcessThread::IsRebootByTime(std::string time)
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
