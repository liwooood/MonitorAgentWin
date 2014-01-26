#ifndef CUSTOM_MESSAGE_H
#define CUSTOM_MESSAGE_H

#include <vector>

#include <boost/array.hpp>
#include <boost/scoped_array.hpp> 
#include <boost/noncopyable.hpp>
#include <boost/checked_delete.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "MsgHeader.h"
//#include "tcpsession.h"


/*
独立出来的原因，
1.以后可以改由内存池分配
2.异步调用的时候适合放入队列
*/



class CustomMessage
{
public:
	CustomMessage();

private:
	// 消息头
	struct MsgHeader m_MsgHeader;
	//std::vector<char> m_MsgHeader;

	// 消息内容
	std::vector<char> m_MsgContent;


	

public:
	

	
	void SetMsgHeader(unsigned char MsgType, int FunctionNo, unsigned char zip=0);
	bool ParseMsgHeader();
	PMSG_HEADER GetMsgHeader();
	size_t GetMsgHeaderSize();


	void SetMsgContent(std::string content);
	char * GetMsgContent();
	size_t GetMsgContentSize();
	std::string GetMsgContentString();

	void destroy();
	

};



#endif