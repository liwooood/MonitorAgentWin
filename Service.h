#pragma once
#include <string>
#include <vector>

class CService
{
public:
	CService(void);
	~CService(void);

	bool m_bEnable;
	std::string m_sServiceName;
	std::string m_sProcess;
	std::string m_sRebootDate;
	std::string m_sRebootTime;

	std::string m_sTcpServer;
	std::string m_sSslServer;
	//std::vector<int> m_vPort;
};

