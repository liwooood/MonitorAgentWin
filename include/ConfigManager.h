#pragma once
#include <boost/thread/detail/singleton.hpp>


#include "Service.h"
#include <string>

class CConfigManager
{
public:
	CConfigManager(void);
	~CConfigManager(void);

public:
	std::string GetWorkingDir();
	std::string GetConfigFile();
	bool LoadConfig();
	

	std::string m_sPath;
	std::string m_sConfigFile;

	std::vector<CService> m_vService;

	int m_nProcessCheck;
	int m_nServiceCheck;//暂时没用到
	int m_nServiceInit;
	int m_nConnectTimeout;
	int m_nReadWriteTimeout;

	int logLevel;

};
typedef boost::detail::thread::singleton<CConfigManager> sConfigManager;

