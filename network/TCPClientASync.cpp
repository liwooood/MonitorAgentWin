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

#include "output/FileLog.h"
#include "config/ConfigManager.h"


/*
CTCPClientASync::CTCPClientASync(void)
	:work(ios)
	,socket(ios)
	,deadline(ios)
{
	
	m_bConnected = false;

	//m_nConnectTimeout = 3;
	//m_nReadWriteTimeout = 6;

	deadline.expires_at(boost::posix_time::pos_infin);
	
	check_deadline();
}

void CTCPClientASync::check_deadline()
{
	if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		gFileLog::instance().Log("���ӳ�ʱ���д��ʱ");

		Close();

		deadline.expires_at(boost::posix_time::pos_infin);
	}
	
	deadline.async_wait( boost::bind(&CTCPClientASync::check_deadline, this) );
}


CTCPClientASync::~CTCPClientASync(void)
{
}

bool CTCPClientASync::Connect(std::string ip, int port)
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


bool CTCPClientASync::IsConnected()
{
	return m_bConnected;
}



void CTCPClientASync::Write(TCPClientMsg * pPkg)
{
	// ���ö�д��ʱ
	//int nReadWriteTimeout = sConfigManager::instance().m_nReadWriteTimeout;
	//deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	

	boost::asio::async_write(socket, 
		boost::asio::buffer(pPkg->GetPkgHeader(), pPkg->GetPkgHeaderSize()), 
		boost::asio::transfer_all(), 
		bind(&CTCPClientASync::WritePkgHeaderHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pPkg) );
}


// д��ͷ
void CTCPClientASync::WritePkgHeaderHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg)
{
	if (error)
	{
		std::string sErrInfo = "д��ͷʧ�ܣ�������룺" + boost::lexical_cast<int>(error.value());
		sErrInfo += ", ������Ϣ��" + error.message();
		gFileLog::instance().Log(sErrInfo);

		Close();
		return;
	}

	pPkg->SumPkgHeaderTransferredBytes(nTransferredBytes);
	if (pPkg->GetPkgHeaderTransferredBytes() != pPkg->GetPkgHeaderSize())
	{
		std::string sErrInfo = "д��ͷʧ�ܣ���д�ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgHeaderTransferredBytes());
		sErrInfo += ", ʵ���ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgHeaderSize());
		gFileLog::instance().Log(sErrInfo);

		// ������ѭ����ȡ
		//return;
	}

	
	boost::asio::async_write(socket, 
		boost::asio::buffer(pPkg->GetPkgBody(), pPkg->GetPkgBodySize()), 
		boost::asio::transfer_all(), 
		bind(&CTCPClientASync::WritePkgBodyHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pPkg) );
}

// д������
void CTCPClientASync::WritePkgBodyHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg)
{
	if (error)
	{
		std::string sErrInfo = "д������ʧ�ܣ�������룺" + boost::lexical_cast<int>(error.value());
		sErrInfo += ", ������Ϣ��" + error.message();
		gFileLog::instance().Log(sErrInfo);

		Close();
		return;
	}

	pPkg->SumPkgBodyTransferredBytes(nTransferredBytes);
	if (pPkg->GetPkgBodyTransferredBytes() != pPkg->GetPkgBodySize())
	{
		std::string sErrInfo = "д������ʧ�ܣ���д�ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgBodyTransferredBytes());
		sErrInfo += ", ʵ���ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgBodySize());
		gFileLog::instance().Log(sErrInfo);

		// ������ѭ����ȡ
		//return;
	}

	// �ͷ���Դ
	delete pPkg;

	// ����Ӧ��
	Read();
}

// ��Ӧ���
void CTCPClientASync::Read()
{
	TCPClientMsg * pRes = new TCPClientMsg();

	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetPkgHeader(), pRes->GetPkgHeaderSize()), 
		boost::asio::transfer_all(), 
		bind(&CTCPClientASync::ReadPkgHeaderHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pRes) );
}

// ��Ӧ���ͷ
void CTCPClientASync::ReadPkgHeaderHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg)
{
	if (error)
	{
		std::string sErrInfo = "��Ӧ���ͷʧ�ܣ�������룺" + boost::lexical_cast<int>(error.value());
		sErrInfo += ", ������Ϣ��" + error.message();
		gFileLog::instance().Log(sErrInfo);

		Close();
		return;
	}

	pPkg->SumPkgHeaderTransferredBytes(nTransferredBytes);
	if (pPkg->GetPkgHeaderTransferredBytes() != pPkg->GetPkgHeaderSize())
	{
		std::string sErrInfo = "��Ӧ���ͷʧ�ܣ��Ѷ��ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgHeaderTransferredBytes());
		sErrInfo += ", ʵ���ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgHeaderSize());
		gFileLog::instance().Log(sErrInfo);

		// ������ѭ����ȡ
	}

	if (!pPkg->DecodePkgHeader())
	{
		Close();
		return;
	}
		
	boost::asio::async_read(socket, 
		boost::asio::buffer(pPkg->GetPkgBody(), pPkg->GetPkgBodySize()), 
		boost::asio::transfer_all(), 
		bind(&CTCPClientASync::ReadPkgBodyHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pPkg) );
}

// ��Ӧ�������
void CTCPClientASync::ReadPkgBodyHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg)
{
	if (error)
	{
		std::string  sErrInfo = "��Ӧ�������ʧ�ܣ�������룺" + boost::lexical_cast<int>(error.value());
		sErrInfo += ", ������Ϣ��" + error.message();
		gFileLog::instance().Log(sErrInfo);

		Close();
		return;
	}

	pPkg->SumPkgBodyTransferredBytes(nTransferredBytes);
	if (pPkg->GetPkgBodyTransferredBytes() != pPkg->GetPkgBodySize())
	{
		std::string sErrInfo = "��Ӧ�������ʧ�ܣ��Ѷ��ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgBodyTransferredBytes());
		sErrInfo += ", ʵ���ֽڣ�" + boost::lexical_cast<int>(pPkg->GetPkgBodySize());
		gFileLog::instance().Log(sErrInfo);

		// ������ѭ����ȡ
	}

	std::string response(pPkg->GetPkgBody().begin(), pPkg->GetPkgBody().end());
	gFileLog::instance().Log("Ӧ�����ݣ�" + response);

	// �ͷ���Դ
	delete pPkg;
}

// �ر�����
void CTCPClientASync::Close()
{
	gFileLog::instance().Log("�ر�����!");

	m_bConnected = false;

	boost::system::error_code ec;

	socket.close(ec);

	//socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	
	//if (ec)
	//{
	//	gFileLog::instance().Log("�Ͽ����������쳣��" + ec.message());
	//}

	//
}

bool CTCPClientASync::ReConnect()
{
	Close();

	return Connect(m_sIP, m_nPort);
}

std::string CTCPClientASync::decompress(std::string data)
{
    std::stringstream compressed;
    compressed << data;

	
    
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_decompressor());
    out.push(compressed);

	std::stringstream decompressed;
    boost::iostreams::copy(out, decompressed);

    return decompressed.str();
}

// ���Ͱ�
void CTCPClientASync::HeartBeat()
{
	if (!m_bConnected)
		return;

	std::string SOH = "\x01";

	std::string  sRequest = "cssweb_funcid=999999" + SOH;

	TCPClientMsg * pReq = new TCPClientMsg();
	pReq->SetPkgBody(sRequest);
	pReq->EncodePkgHeader();

	Write(pReq);
}

void CTCPClientASync::init()
{
	boost::thread t(boost::bind(&boost::asio::io_service::run, &ios));
		
}
*/
