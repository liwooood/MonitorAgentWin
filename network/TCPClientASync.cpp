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

#include "TCPClientASync.h"

#include "FileLog.h"
#include "ConfigManager.h"
#include "tcp_message_old.h"



CTCPClientASync::CTCPClientASync(void)
	:work(ios)
	,socket(ios)
	,deadline(ios)
	,stopped(false)
{
	deadline.expires_at(boost::posix_time::pos_infin);
}


CTCPClientASync::~CTCPClientASync(void)
{
}

// OK
void CTCPClientASync::check_deadline()
{
	if (stopped)
		return;

	if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		//gFileLog::instance().Log("���ӳ�ʱ���д��ʱ");

		socket.close();

		deadline.expires_at(boost::posix_time::pos_infin);
	}
	
	deadline.async_wait( boost::bind(&CTCPClientASync::check_deadline, this) );
}


// �ر�����
void CTCPClientASync::Close()
{
	stopped = true;


	boost::system::error_code ec;

	socket.close(ec);
	deadline.cancel();
}



void CTCPClientASync::Connect(std::string ip, int port)
{
	
	
	// �������ӳ�ʱ
		int nConnectTimeout = gConfigManager::instance().m_nConnectTimeout;
		deadline.expires_from_now( boost::posix_time::seconds(nConnectTimeout) );

		m_sIP = ip;
		m_nPort = port;

		boost::system::error_code ec;

		boost::asio::ip::tcp::resolver resolver(ios);

		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), m_sIP, boost::lexical_cast<std::string>(m_nPort));

		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);

		socket.async_connect(iterator->endpoint(), boost::bind(&CTCPClientASync::OnConnect, this, boost::asio::placeholders::error) );
		
		//deadline.async_wait(boost::bind(&CTCPClientASync::check_deadline, this));
}

void CTCPClientASync::OnConnect(const boost::system::error_code& ec)
{
	if (ec)
	{
		TRACE("����ʧ��\n");
		Close();
		return;
	}

	if (!socket.is_open())
	{
		TRACE("����ʧ��\n");
		Close();
	}
	else
	{
		TRACE("���ӳɹ�\n");
	}
}


void CTCPClientASync::Write(IMessage * msg)
{
	if (stopped)
		return;

	// ���ö�д��ʱ
	//int nReadWriteTimeout = sConfigManager::instance().m_nReadWriteTimeout;
	//deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	

	boost::asio::async_write(socket, 
		boost::asio::buffer(msg->GetMsgHeader(), msg->GetMsgHeaderSize()), 
		boost::asio::transfer_all(), 
		boost::bind(&CTCPClientASync::OnWriteMsgHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, msg) );
}


// д��ͷ
void CTCPClientASync::OnWriteMsgHeader(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg)
{
	if (stopped)
		return;

	if (error)
	{
		

		Close();
		return;
	}

	

	
	boost::asio::async_write(socket, 
		boost::asio::buffer(msg->GetMsgContent(), msg->GetMsgContentSize()), 
		boost::asio::transfer_all(), 
		bind(&CTCPClientASync::OnWriteMsgContent, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, msg) );
}

// д������
void CTCPClientASync::OnWriteMsgContent(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg)
{
	if (stopped)
		return;

	if (error)
	{
		

		Close();
		return;
	}

	// �ͷ���Դ
	delete msg;

	
}

// ��Ӧ���
void CTCPClientASync::Read(IMessage * msg)
{
	if (msg == NULL)
		return;

	 //deadline.expires_from_now(boost::posix_time::seconds(30));

	boost::asio::async_read(socket, 
		boost::asio::buffer(msg->GetMsgHeader(), msg->GetMsgHeaderSize()), 
		boost::asio::transfer_all(), 
		boost::bind(&CTCPClientASync::OnReadMsgHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, msg) );
}

// ��Ӧ���ͷ
void CTCPClientASync::OnReadMsgHeader(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg)
{
	if (stopped)
		return;

	if (error)
	{
		

		Close();
		return;
	}

	

	if (!msg->DecoderMsgHeader())
	{
		Close();
		return;
	}
		
	boost::asio::async_read(socket, 
		boost::asio::buffer(msg->GetMsgContent(), msg->GetMsgContentSize()), 
		boost::asio::transfer_all(), 
		boost::bind(&CTCPClientASync::OnReadMsgContent, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, msg) );
}

// ��Ӧ�������
void CTCPClientASync::OnReadMsgContent(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg)
{
	if (stopped)
		return;

	if (error)
	{
		Close();
		return;
	}

	

	std::string response(msg->GetMsgContent().begin(), msg->GetMsgContent().end());
	TRACE("response=");
	TRACE(response.c_str());

	// �ͷ���Դ
	delete msg;
}


// ���Ͱ�
void CTCPClientASync::HeartBeat()
{
	std::string SOH = "\x01";

	std::string request = "cssweb_funcid=999999" + SOH;

	bool bRet = false;

	
	// ���ö�д��ʱ
	//int nReadWriteTimeout = gConfigManager::instance().m_nReadWriteTimeout;
	//deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	// ��������
	IMessage * pReq = new tcp_message_old();

	int msgHeaderSize = request.size();
		msgHeaderSize = htonl(msgHeaderSize);
		memcpy(&(pReq->m_MsgHeader.front()), &msgHeaderSize, 4);

	pReq->SetMsgContent(request);


	

	Write(pReq);
	

	// ����Ӧ��
	IMessage * pRes = new tcp_message_old();
	Read(pRes);
	

}

void CTCPClientASync::init()
{
	boost::thread t(boost::bind(&boost::asio::io_service::run, &ios));
		
}

