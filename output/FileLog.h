#ifndef _FILE_LOG_
#define _FILE_LOG_


#include <string>

#include <boost/thread/detail/singleton.hpp>

/*
��������boost::log����
*/

class FileLog
{
public:
	FileLog(void);
	~FileLog(void);

	void Log(std::string log, std::string file="debug.log");
};

typedef boost::detail::thread::singleton<FileLog> gFileLog;

#endif