#include "stdafx.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "ConfigManager.h"
#include "xml/pugixml.hpp"


CConfigManager::CConfigManager(void)
{
}

CConfigManager::~CConfigManager(void)
{
}

std::string CConfigManager::GetWorkingDir()
{
	char szPath[255];
	memset(szPath, 0x00, sizeof(szPath));
	::GetModuleFileName(NULL, szPath, sizeof(szPath));
	

	boost::filesystem::path p(szPath);
	m_sPath = p.parent_path().string();

	return m_sPath;
}

std::string CConfigManager::GetConfigFile()
{
	GetWorkingDir();

	m_sConfigFile = m_sPath + "\\config.xml";

	return m_sConfigFile;
}

bool CConfigManager::LoadConfig()
{
	GetConfigFile();

	pugi::xml_document doc;
	if (!doc.load_file(m_sConfigFile.c_str()))
		return false;

	pugi::xpath_node node;
	
	
	pugi::xpath_node_set nodes;
	
	

	
	node = doc.select_single_node("/config/process_check");
	m_nProcessCheck = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/service_check");
	m_nServiceCheck = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/service_init");
	m_nServiceInit = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/connect_timeout");
	m_nConnectTimeout = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/readwrite_timeout");
	m_nReadWriteTimeout = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/LogLevel");
	logLevel = boost::lexical_cast<int>(node.node().child_value());


	CService service;

	node = doc.select_single_node("/config/service/enable");
	int nEnable = boost::lexical_cast<int>(node.node().child_value());
	service.m_bEnable = nEnable;

	node = doc.select_single_node("/config/service/name");
	std::string name = node.node().child_value();
	service.m_sServiceName = name;

	node = doc.select_single_node("/config/service/process");
	std::string process = node.node().child_value();
	service.m_sProcess = process;

	node = doc.select_single_node("/config/service/reboot_date");
	std::string reboot_date = node.node().child_value();
	service.m_sRebootDate = reboot_date;

	node = doc.select_single_node("/config/service/reboot_time");
	std::string reboot_time = node.node().child_value();
	service.m_sRebootTime = reboot_time;


	node = doc.select_single_node("/config/service/trade_ssl");
	std::string server = node.node().child_value();
	service.m_sTcpServer = server;

	node = doc.select_single_node("/config/service/trade_tcp");
	server = node.node().child_value();
	service.m_sSslServer = server;

	/*
	nodes = doc.select_nodes("/config/service/port");

	pugi::xpath_node_set::const_iterator it;

	for (it= nodes.begin(); it != nodes.end(); ++it)
	{
		pugi::xpath_node node = *it;

		int port = boost::lexical_cast<int>(node.node().child_value());
		service.m_vPort.push_back(port);
	}
	*/
	m_vService.push_back(service);

	return true;
}


