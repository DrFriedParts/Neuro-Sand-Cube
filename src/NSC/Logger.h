#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <fstream>

class Logger
{

public:
	
	static Logger& GetInstance();
	~Logger(void);

	enum LogLevel
	{
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR
	};

	//template <typename T>
	void Log(std::string message,LogLevel level = LOG_ERROR);

private:
	Logger(void);

	//static Logger& m_Instance;
	std::ofstream m_File;

};

#endif