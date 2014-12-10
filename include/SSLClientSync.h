#ifndef SSL_CLIENT_SYNC_H
#define SSL_CLIENT_SYNC_H

// stl
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/scoped_array.hpp> 
#include <boost/noncopyable.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/system/system_error.hpp>

#include "MsgHeader.h"
#include "CustomMessage.h"



/*
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/ssl/client.cpp
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/timeouts/blocking_tcp_client.cpp
*/

class SSLClientSync
{
public:
	SSLClientSync(boost::asio::ssl::context& context);
	~SSLClientSync(void);

	bool Connect(std::string ip, int port);
	void Close();

	bool ReConnect();
	bool IsConnected();

	// 心跳功能，由于后台业务不同，所以封装
	bool HeartBeat();

	//bool Send(std::string& request, std::string& response);

	void SetConnectTimeout(int seconds);
	void SetReadWriteTimeout(int seconds);

private:
	bool Write(IMessage * pReq);
	bool WriteMsgHeader(IMessage * pReq);
	bool WriteMsgContent(IMessage * pReq);

	bool Read(IMessage * pRes);
	bool ReadMsgHeader(IMessage * pRes);
	bool ReadMsgContent(IMessage * pRes);

	void check_deadline();
	 bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);

private:
	boost::asio::io_service ios;

	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;
	//boost::asio::ssl::context context;

	boost::asio::deadline_timer deadline;
	

	std::string m_sIP;
	int m_nPort;

	bool m_bConnected;

	int connectTimeout;
	int readWriteTimeout;
};
#endif
