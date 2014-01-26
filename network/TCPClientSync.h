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

#include "network/MsgHeader.h"
#include "network/CustomMessage.h"

/*
问题一：超时
问题二：循环读写
问题三：运行时间
*/
class CTCPClientSync
{
public:
	CTCPClientSync();
	~CTCPClientSync(void);

	bool Connect(std::string ip, int port);
	void Close();

	bool ReConnect();
	bool IsConnected();

	bool HeartBeat();

private:
	bool Write(CustomMessage * pReq);
	bool WriteMsgHeader(CustomMessage * pReq);
	bool WriteMsgContent(CustomMessage * pReq);

	bool Read(CustomMessage * pRes);
	bool ReadMsgHeader(CustomMessage * pRes);
	bool ReadMsgContent(CustomMessage * pRes);

	void check_deadline();
	std::string decompress(std::string data);

private:
	boost::asio::io_service ios;
	boost::asio::ip::tcp::socket socket;
	boost::asio::deadline_timer deadline;
	

	std::string m_sIP;
	int m_nPort;

	bool m_bConnected;
};
