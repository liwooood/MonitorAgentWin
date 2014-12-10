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

#include "MsgHeader.h"
#include "CustomMessage.h"

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
	bool Write(IMessage * pReq);
	bool WriteMsgHeader(IMessage * pReq);
	bool WriteMsgContent(IMessage * pReq);

	bool Read(IMessage * pRes);
	bool ReadMsgHeader(IMessage * pRes);
	bool ReadMsgContent(IMessage * pRes);

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
