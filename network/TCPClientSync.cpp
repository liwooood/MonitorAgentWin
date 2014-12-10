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
#include "FileLog.h"
#include "ConfigManager.h"
#include "tcp_message_old.h"



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

// 超时回调函数
void CTCPClientSync::check_deadline()
{
	
	if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		gFileLog::instance().Log(LOG_LEVEL_ERROR, "TCP连接超时或读写超时");

		Close();

		deadline.expires_at(boost::posix_time::pos_infin);
	}
	
	deadline.async_wait( boost::bind(&CTCPClientSync::check_deadline, this) );
}

// 建立连接
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

		// 设置连接超时
		int nConnectTimeout = sConfigManager::instance().m_nConnectTimeout;
		std::string sConnectTimeout = boost::lexical_cast<std::string>(nConnectTimeout);
		//gFileLog::instance().Log(LOG_LEVEL_DEBUG, "TCP连接超时时间:" + sConnectTimeout);

		deadline.expires_from_now( boost::posix_time::seconds(nConnectTimeout) );

		ec = boost::asio::error::would_block;

		boost::asio::async_connect(socket, iterator, boost::lambda::var(ec) = boost::lambda::_1);
	
		do 
			ios.run_one(); 
		while (ec == boost::asio::error::would_block);

		if (ec || !socket.is_open())
		{
			std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
			std::string sErrMsg = ec.message();
			std::string sErrInfo = "TCP连接交易网关失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
			gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);
			
			
			m_bConnected = false;
			return m_bConnected;
		}

		gFileLog::instance().Log(LOG_LEVEL_DEBUG, "TCP连接交易网关成功!");
		m_bConnected = true;
		return m_bConnected;
	}
	catch(std::exception& e)
	{
		gFileLog::instance().Log(LOG_LEVEL_ERROR, "TCP连接交易网关异常：" + std::string(e.what()));
		m_bConnected = false;
		return m_bConnected;
	}
}

bool CTCPClientSync::IsConnected()
{
	return m_bConnected;
}



bool CTCPClientSync::Write(IMessage * pReq)
{
	if (!WriteMsgHeader(pReq))
		return false;

	if (!WriteMsgContent(pReq))
		return false;

	return true;
}

// 写包头
bool CTCPClientSync::WriteMsgHeader(IMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	
	
	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgHeader(), pReq->GetMsgHeaderSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "TCP写包头失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool CTCPClientSync::WriteMsgContent(IMessage * pReq)
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
		std::string sErrInfo = "TCP写包内容失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool CTCPClientSync::Read(IMessage * pRes)
{
	if (!ReadMsgHeader(pRes))
		return false;

	if (!ReadMsgContent(pRes))
		return false;

	
	return true;
}

// 读包头
bool CTCPClientSync::ReadMsgHeader(IMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgHeader(), pRes->GetMsgHeaderSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "TCP读包头失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

// 读包内容
bool CTCPClientSync::ReadMsgContent(IMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	if (!pRes->DecoderMsgHeader())
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
		std::string sErrInfo = "TCP读包内容失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;			
	}
		
	m_bConnected = true;
	return m_bConnected;
}

// 关闭连接
void CTCPClientSync::Close()
{
	m_bConnected = false;

	boost::system::error_code ec;

	
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	socket.close(ec);
	
	if (ec)
	{
		gFileLog::instance().Log(LOG_LEVEL_ERROR, "TCP断开交易网关异常：" + ec.message());
	}

	
	gFileLog::instance().Log(LOG_LEVEL_DEBUG, "TCP断开交易网关!");
}

bool CTCPClientSync::ReConnect()
{
	Close();

	return Connect(m_sIP, m_nPort);
}

// 发送心跳包
bool CTCPClientSync::HeartBeat()
{
	if (!m_bConnected)
		return false;

	std::string SOH = "\x01";

	std::string request = "cssweb_funcid=999999" + SOH;

	bool bRet = false;

	
	// 设置读写超时
	int nReadWriteTimeout = sConfigManager::instance().m_nReadWriteTimeout;
	deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	// 发送请求
	IMessage * pReq = new tcp_message_old();

	int msgHeaderSize = request.size();
		msgHeaderSize = htonl(msgHeaderSize);
		memcpy(&(pReq->m_MsgHeader.front()), &msgHeaderSize, 4);

	pReq->SetMsgContent(request);


	

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
		return false;

	// 接收应答
	IMessage * pRes = new tcp_message_old();
	bRet = Read(pRes);
	if (bRet)
	{
		//std::string response(pRes->GetPkgBody().begin(),pRes->GetPkgBody().end());
		gFileLog::instance().Log(LOG_LEVEL_DEBUG, "TCP应答内容：" + pRes->GetMsgContentString());
		bRet = true;
	}
	else
	{
		bRet = false;
	}
	delete pRes;	

	
	
	return bRet;
}
