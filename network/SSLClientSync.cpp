#include "SSLClientSync.h"


#include "FileLog.h"
#include "ConfigManager.h"
#include "ssl_message.h"




SSLClientSync::SSLClientSync(boost::asio::ssl::context& context)
	
	:socket(ios, context)
	,deadline(ios)
{
	
	m_bConnected = false;


	deadline.expires_at(boost::posix_time::pos_infin);
	
	check_deadline();

	//socket.set_verify_mode(boost::asio::ssl::verify_peer);
	//socket.set_verify_callback(boost::bind(&SSLClientSync::verify_certificate, this, _1, _2));
}

 bool SSLClientSync::verify_certificate(bool preverified,boost::asio::ssl::verify_context& ctx)
 {
	// The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";

    return preverified;
 }

SSLClientSync::~SSLClientSync(void)
{
}

// ��ʱ�ص�����
void SSLClientSync::check_deadline()
{
	
	if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		gFileLog::instance().Log(LOG_LEVEL_ERROR, "SSL���ӳ�ʱ���д��ʱ");

		//m_bConnected = false;
		Close();

		deadline.expires_at(boost::posix_time::pos_infin);
	}
	
	deadline.async_wait( boost::bind(&SSLClientSync::check_deadline, this) );
}

// ��������
bool SSLClientSync::Connect(std::string ip, int port)
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
		int nConnectTimeout = gConfigManager::instance().m_nConnectTimeout;
		deadline.expires_from_now( boost::posix_time::seconds(nConnectTimeout) );

		ec = boost::asio::error::would_block;

		boost::asio::async_connect(socket.lowest_layer(), iterator, boost::lambda::var(ec) = boost::lambda::_1);
	
		do 
			ios.run_one(); 
		while (ec == boost::asio::error::would_block);

		if (ec)
		{
			std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
			std::string sErrMsg = ec.message();
			std::string sErrInfo = "SSL���ӽ�������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
			gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);
			
			
			m_bConnected = false;
			return m_bConnected;
		}

		
		ec = boost::asio::error::would_block;
		socket.async_handshake(boost::asio::ssl::stream_base::client, boost::lambda::var(ec) = boost::lambda::_1);
		do 
			ios.run_one(); 
		while (ec == boost::asio::error::would_block);
		if (ec)
		{
			std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
			std::string sErrMsg = ec.message();
			std::string sErrInfo = "SSL���ӽ�����������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
			gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);
			
			
			m_bConnected = false;
			return m_bConnected;
		}
		

		gFileLog::instance().Log(LOG_LEVEL_DEBUG, "SSL���ӽ������سɹ�!");
		m_bConnected = true;
		return m_bConnected;
	}
	catch(std::exception& e)
	{
		gFileLog::instance().Log(LOG_LEVEL_ERROR, "SSL���ӽ��������쳣��" + std::string(e.what()));
		m_bConnected = false;
		return m_bConnected;
	}
}

bool SSLClientSync::IsConnected()
{
	
	return m_bConnected;
}



bool SSLClientSync::Write(IMessage * pReq)
{
	if (!WriteMsgHeader(pReq))
		return false;

	if (!WriteMsgContent(pReq))
		return false;

	return true;
}

// д��ͷ
bool SSLClientSync::WriteMsgHeader(IMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	int pkgHeaderSize = pReq->GetMsgHeaderSize();
	std::string msg = "��Ҫ���͵İ�ͷ�ֽڴ�С" + boost::lexical_cast<std::string>(pkgHeaderSize);
	gFileLog::instance().Log(LOG_LEVEL_DEBUG, msg);
	
	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgHeader(), pReq->GetMsgHeaderSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "SSLд��ͷʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool SSLClientSync::WriteMsgContent(IMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	int pkgHeaderSize = pReq->GetMsgContentSize();
	std::string msg = "��Ҫ���͵İ����ֽڴ�С" + boost::lexical_cast<std::string>(pkgHeaderSize);
	gFileLog::instance().Log(LOG_LEVEL_DEBUG, msg);

	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgContent(), pReq->GetMsgContentSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "SSLд������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool SSLClientSync::Read(IMessage * pRes)
{
	if (!ReadMsgHeader(pRes))
		return false;

	if (!ReadMsgContent(pRes))
		return false;

	
	return true;
}

// ����ͷ
bool SSLClientSync::ReadMsgHeader(IMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	int pkgHeaderSize = pRes->GetMsgHeaderSize();
	std::string msg = "��Ҫ���յİ�ͷ�ֽڴ�С" + boost::lexical_cast<std::string>(pkgHeaderSize);
	//gFileLog::instance().Log(LOG_LEVEL_DEBUG, msg);

	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgHeader(), pRes->GetMsgHeaderSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "SSL����ͷʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

// ��������
bool SSLClientSync::ReadMsgContent(IMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	if (!pRes->DecoderMsgHeader())
	{
		return false;
	}
	
	int pkgHeaderSize = pRes->GetMsgContentSize();
	std::string msg = "��Ҫ���յİ����ֽڴ�С" + boost::lexical_cast<std::string>(pkgHeaderSize);
	gFileLog::instance().Log(LOG_LEVEL_DEBUG, msg);
	
	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgContent(), pRes->GetMsgContentSize()),
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

		
	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "SSL��������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(LOG_LEVEL_ERROR, sErrInfo);

		m_bConnected = false;
		return m_bConnected;			
	}
		
	m_bConnected = true;
	return m_bConnected;
}

// �ر�����
void SSLClientSync::Close()
{
//	if (m_bConnected)
	{
		m_bConnected = false;

		boost::system::error_code ec;

		//socket.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	
		//socket.lowest_layer().close();
	
		socket.async_shutdown(boost::lambda::var(ec) = boost::lambda::_1);
		do 
			ios.run_one(); 
		while (ec == boost::asio::error::would_block);
	
		if (ec)
		{
			gFileLog::instance().Log(LOG_LEVEL_ERROR, "SSL�Ͽ����������쳣��" + ec.message());
		}
		

	
		gFileLog::instance().Log(LOG_LEVEL_DEBUG, "SSL�Ͽ���������!");
	}
}

void SSLClientSync::shutdown()
{
}

bool SSLClientSync::ReConnect()
{
	Close();

	return Connect(m_sIP, m_nPort);
}

// ����������
bool SSLClientSync::HeartBeat()
{
	if (!m_bConnected)
		return false;

	std::string SOH = "\x01";

	std::string request = "cssweb_funcid=999999" + SOH;

	bool bRet = false;

	// ��ʼʱ��
	 boost::posix_time::ptime time_sent = boost::posix_time::microsec_clock::universal_time();
	
	// ���ö�д��ʱ
	int nReadWriteTimeout = gConfigManager::instance().m_nReadWriteTimeout;
	deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	// ��������
	IMessage * pReq = new ssl_message();

	quote::PkgHeader pbHeader;
	pbHeader.set_zip(false);
	pbHeader.set_bodysize(request.size());
	pbHeader.set_ver(1);
	pbHeader.set_enc(false);
	pbHeader.set_more(false);
	pbHeader.set_msgtype(quote::PkgHeader::REQ_TRADE);
	pbHeader.set_errcode(0);
	pbHeader.SerializeToArray(&(pReq->m_MsgHeader.front()), pbHeader.ByteSize());		

	pReq->SetMsgContent(request);


	

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
		return false;

	// ����Ӧ��
	IMessage * pRes = new ssl_message();
	bRet = Read(pRes);
	if (bRet)
	{
		//std::string response(pRes->GetPkgBody().begin(),pRes->GetPkgBody().end());
		gFileLog::instance().Log(LOG_LEVEL_DEBUG, "SSLӦ�����ݣ�" + pRes->GetMsgContentString());
		bRet = true;
	}
	else
	{
		//bRet = false;
		bRet = true; // ����short read���󣬲��жϽ���
	}
	delete pRes;	

	// ����ʱ��
	boost::posix_time::ptime time_received = boost::posix_time::microsec_clock::universal_time();

	// ����ʱ��
	//int nRuntime = (time_received - time_sent).total_microseconds();
	//gFileLog::instance().Log("ִ��ʱ�䣺" + boost::lexical_cast<std::string>(nRuntime));

	return bRet;
}

void SSLClientSync::SetConnectTimeout(int connecTimeout)
{
	this->connectTimeout = connectTimeout;
}

void SSLClientSync::SetReadWriteTimeout(int readWriteTimeout)
{
	this->readWriteTimeout = readWriteTimeout;
}
/*
bool SSLClientSync::Send(std::string& request, std::string& response)
{
	bool bRet = false;

	if (request.empty())
		return bRet;
	

	// ��������
	CustomMessage * pReq = new CustomMessage(MSG_TYPE_SSL_NEW);

	

	pReq->SetMsgContent(request);
	//pReq->SetMsgHeader();

	int temp = pReq->GetMsgHeaderSize();

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
		return false;

	// ����Ӧ��
	CustomMessage * pRes = new CustomMessage(MSG_TYPE_SSL_NEW);
	bRet = Read(pRes);
	if (bRet)
	{
		response = pRes->GetMsgContentString();
		//std::string response(pRes->GetPkgBody().begin(),pRes->GetPkgBody().end());
		gFileLog::instance().Log(LOG_LEVEL_ERROR, "Ӧ�����ݣ�" + pRes->GetMsgContentString());
	}
	else
	{
	}
	delete pRes;	

	return true;
}
*/