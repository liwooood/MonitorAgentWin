#ifndef _FILE_LOG_
#define _FILE_LOG_


#include <string>

#include <boost/thread/detail/singleton.hpp>

/*
将来采用boost::log代替
*/

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

class FileLog
{
public:
	FileLog(void);
	~FileLog(void);

	void Log(int logLevel, std::string log, std::string file="debug.log");
};

typedef boost::detail::thread::singleton<FileLog> gFileLog;

#endif