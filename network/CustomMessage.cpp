#include "CustomMessage.h"


CustomMessage::CustomMessage()
{
	
}

void CustomMessage::SetMsgContent(std::string content)
{
	size_t MsgContentSize = content.size();

	if (MsgContentSize == 0)
		return;

	m_MsgContent.resize(MsgContentSize);

	//std::copy(content.begin(), content.end(), back_inserter(m_MsgContent));

	memcpy(m_MsgContent.data(), content.c_str(), MsgContentSize);
}

void CustomMessage::SetMsgHeader(unsigned char MsgType, int FunctionNo, unsigned char zip)
{
	memset(&m_MsgHeader, 0, sizeof(struct MsgHeader));

	//int nMsgContentSize = htonl(m_MsgContent.size());
	m_MsgHeader.MsgContentSize = m_MsgContent.size();

	m_MsgHeader.CRC = 0;
	m_MsgHeader.MsgType = MsgType;
	m_MsgHeader.FunctionNo = FunctionNo;
	m_MsgHeader.zip = zip;

}



bool CustomMessage::ParseMsgHeader()
{
	bool bRet = true;
	
	// java是网络字序，c++是主机字序，所以需要转换
	//int MsgContentSize = ntohl(m_MsgHeader.MsgContentSize);
	//m_MsgHeader.MsgContentSize = MsgContentSize;


	m_MsgContent.resize(m_MsgHeader.MsgContentSize);

	return bRet;
}

PMSG_HEADER CustomMessage::GetMsgHeader()
{
	return &m_MsgHeader;
}

char * CustomMessage::GetMsgContent()
{
	return m_MsgContent.data();
}

size_t CustomMessage::GetMsgContentSize()
{
	return m_MsgContent.size();
}

std::string CustomMessage::GetMsgContentString()
{
	std::string str(m_MsgContent.begin(), m_MsgContent.end());
	return str;

}

size_t CustomMessage::GetMsgHeaderSize()
{
	return sizeof(MSG_HEADER);
}

// 删除自己
void CustomMessage::destroy()
{
	boost::checked_delete(this);
}

