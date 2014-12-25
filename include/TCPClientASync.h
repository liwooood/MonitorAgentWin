#ifndef TCP_CLIENT_ASYNC_H
#define TCP_CLIENT_ASYNC_H

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

#include "IMessage.h"

/*
问题一：超时
问题二：循环读写
*/
class CTCPClientASync
{
	
public:
	CTCPClientASync();
	~CTCPClientASync(void);

	void init();

	void Connect(std::string ip, int port);
	
	void Close();

	bool ReConnect();
	bool IsConnected();

	void HeartBeat();

	  
	void Write(IMessage * msg);
	void Read();

private:
	void OnConnect(const boost::system::error_code& ec);

	
	void OnWriteMsgHeader(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg);
	void OnWriteMsgContent(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg);

	
	void OnReadMsgHeader(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg);
	void OnReadMsgContent(const boost::system::error_code& error, int nTransferredBytes, IMessage * msg);
	
	void check_deadline();

	

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
	bool stopped;
	
};
#endif
