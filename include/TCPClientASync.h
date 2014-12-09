#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/scoped_array.hpp> 
#include <boost/noncopyable.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/deadline_timer.hpp>

/*
问题一：超时
问题二：循环读写
*/
class CTCPClientASync
{
	/*
public:
	CTCPClientASync();
	~CTCPClientASync(void);

	void init();

	bool Connect(std::string ip, int port);
	
	void Close();

	bool ReConnect();
	bool IsConnected();

	void HeartBeat();

private:
	void Write(TCPClientMsg * pPkg);
	void WritePkgHeaderHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg);
	void WritePkgBodyHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg);

	void Read();
	void ReadPkgHeaderHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg);
	void ReadPkgBodyHandler(const boost::system::error_code& error, int nTransferredBytes, TCPClientMsg * pPkg);
	
	void check_deadline();

	std::string decompress(std::string data);

private:
	boost::asio::io_service ios;
	boost::asio::io_service::work work;

	boost::asio::ip::tcp::socket socket;
	boost::asio::deadline_timer deadline;
	

	std::string m_sIP;
	int m_nPort;

	bool m_bConnected;

	//int m_nConnectTimeout;
	//int m_nReadWriteTimeout;
	*/
};
