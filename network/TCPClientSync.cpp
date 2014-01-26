#include "stdafx.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/system/system_error.hpp>

#include "TCPClientSync.h"
#include "output/FileLog.h"
#include "config/ConfigManager.h"



CTCPClientSync::CTCPClientSync(void)
	:socket(ios)
	,deadline(ios)
{
	
	m_bConnected = false;


	deadline.expires_at(boost::posix_time::pos_infin);
	
	check_deadline();
}

CTCPClientSync::~CTCPClientSync(void)
{
}

// ��ʱ�ص�����
void CTCPClientSync::check_deadline()
{
	
	if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		gFileLog::instance().Log("���ӳ�ʱ���д��ʱ");

		Close();

		deadline.expires_at(boost::posix_time::pos_infin);
	}
	
	deadline.async_wait( boost::bind(&CTCPClientSync::check_deadline, this) );
}

// ��������
bool CTCPClientSync::Connect(std::string ip, int port)
{
	try
	{
		m_bConnected = false;
	
		m_sIP = ip;
		m_nPort = port;

		boost::system::error_code ec;

		boost::asio::ip::tcp::resolver resolver(ios);

		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), m_sIP, boost::lexical_cast<std::string>(m_nPort));

		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);

		// �������ӳ�ʱ
		//int nConnectTimeout = sConfigManager::instance().m_nConnectTimeout;
		//deadline.expires_from_now( boost::posix_time::seconds(nConnectTimeout) );

		ec = boost::asio::error::would_block;

		boost::asio::async_connect(socket, iterator, boost::lambda::var(ec) = boost::lambda::_1);
	
		do 
			ios.run_one(); 
		while (ec == boost::asio::error::would_block);

		if (ec || !socket.is_open())
		{
			std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
			std::string sErrMsg = ec.message();
			std::string sErrInfo = "���ӽ�������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
			gFileLog::instance().Log(sErrInfo);
			
			
			m_bConnected = false;
			return m_bConnected;
		}

		gFileLog::instance().Log("���ӽ������سɹ�!");
		m_bConnected = true;
		return m_bConnected;
	}
	catch(std::exception& e)
	{
		gFileLog::instance().Log("���ӽ��������쳣��" + std::string(e.what()));
		m_bConnected = false;
		return m_bConnected;
	}
}

bool CTCPClientSync::IsConnected()
{
	return m_bConnected;
}



bool CTCPClientSync::Write(CustomMessage * pReq)
{
	if (!WriteMsgHeader(pReq))
		return false;

	if (!WriteMsgContent(pReq))
		return false;

	return true;
}

// д��ͷ
bool CTCPClientSync::WriteMsgHeader(CustomMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	
	
	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgHeader(), sizeof(MSG_HEADER)), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "д��ͷʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool CTCPClientSync::WriteMsgContent(CustomMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgContent(), pReq->GetMsgContentSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "д������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool CTCPClientSync::Read(CustomMessage * pRes)
{
	if (!ReadMsgHeader(pRes))
		return false;

	if (!ReadMsgContent(pRes))
		return false;

	
	return true;
}

// ����ͷ
bool CTCPClientSync::ReadMsgHeader(CustomMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgHeader(), sizeof(MSG_HEADER)), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "����ͷʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

// ��������
bool CTCPClientSync::ReadMsgContent(CustomMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	if (!pRes->ParseMsgHeader())
	{
		return false;
	}
		
	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgContent(), pRes->GetMsgContentSize()),
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

		
	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "��������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;			
	}
		
	m_bConnected = true;
	return m_bConnected;
}

// �ر�����
void CTCPClientSync::Close()
{
	m_bConnected = false;

	boost::system::error_code ec;

	

	socket.close(ec);
	
	if (ec)
	{
		gFileLog::instance().Log("�Ͽ����������쳣��" + ec.message());
	}

	
	gFileLog::instance().Log("�Ͽ���������!");
}

bool CTCPClientSync::ReConnect()
{
	Close();

	return Connect(m_sIP, m_nPort);
}

// ����������
bool CTCPClientSync::HeartBeat()
{
	if (!m_bConnected)
		return false;

	std::string SOH = "\x01";

	std::string request = "cssweb_funcid=999999" + SOH;

	bool bRet = false;

	// ��ʼʱ��
	 boost::posix_time::ptime time_sent = boost::posix_time::microsec_clock::universal_time();
	
	// ���ö�д��ʱ
	//int nReadWriteTimeout = sConfigManager::instance().m_nReadWriteTimeout;
	//deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	// ��������
	CustomMessage * pReq = new CustomMessage();

	

	pReq->SetMsgContent(request);
	pReq->SetMsgHeader(MSG_TYPE_REQUEST, FUNCTION_HEARTBEAT);

	int temp = pReq->GetMsgHeaderSize();

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
		return false;

	// ����Ӧ��
	CustomMessage * pRes = new CustomMessage();
	bRet = Read(pRes);
	if (bRet)
	{
		//std::string response(pRes->GetPkgBody().begin(),pRes->GetPkgBody().end());
		gFileLog::instance().Log("Ӧ�����ݣ�" + pRes->GetMsgContentString());
	}
	else
	{
	}
	delete pRes;	

	// ����ʱ��
	boost::posix_time::ptime time_received = boost::posix_time::microsec_clock::universal_time();

	// ����ʱ��
	int nRuntime = (time_received - time_sent).total_microseconds();
	//gFileLog::instance().Log("ִ��ʱ�䣺" + boost::lexical_cast<std::string>(nRuntime));

	return bRet;
}
