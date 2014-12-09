#include "stdafx.h"

#include "IMessage.h"
#include <boost/checked_delete.hpp>


IMessage::IMessage()
{
	//m_MsgHeader.resize(msgHeaderSize);
	//this->session = session;
}


IMessage::~IMessage(void)
{
}

std::vector<char>& IMessage::GetMsgHeader()
{
	return m_MsgHeader;
}

size_t IMessage::GetMsgHeaderSize()
{
	return m_MsgHeader.size();
}

void IMessage::SetMsgHeader(std::vector<char> header)
{
	m_MsgHeader = header;
}




std::vector<char>& IMessage::GetMsgContent()
{
	return m_MsgContent;
}


std::string IMessage::GetMsgContentString()
{
	std::string str(m_MsgContent.begin(), m_MsgContent.end());
	return str;

}

size_t IMessage::GetMsgContentSize()
{
	return m_MsgContent.size();
}

void IMessage::SetMsgContent(std::string content)
{
	size_t MsgContentSize = content.size();

	if (MsgContentSize == 0)
		return;

	m_MsgContent.resize(MsgContentSize);

	//std::copy(content.begin(), content.end(), back_inserter(m_MsgContent));

	memcpy(m_MsgContent.data(), content.c_str(), MsgContentSize);
}

void IMessage::SetMsgContent(std::vector<char> content)
{
	size_t MsgContentSize = content.size();

	if (MsgContentSize == 0)
		return;

	m_MsgContent.resize(MsgContentSize);

	
	m_MsgContent = content;
	
}

void IMessage::destroy()
{
	boost::checked_delete(this);
}
