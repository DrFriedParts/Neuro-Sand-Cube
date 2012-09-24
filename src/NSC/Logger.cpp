#include "Logger.h"


#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>


Logger& Logger::GetInstance()
{
	static Logger logger;
	return logger;
}

Logger::Logger(void)
{
	boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());
	
	m_File.open ("NSC_LOG.txt",std::ios::out);
	m_File << t <<" Neuro-Sand-Cube Log file." << std::endl;

}


Logger::~Logger(void)
{
	m_File.close();
}


//template <typename T>
void Logger::Log(std::string message, LogLevel level)
{
	if (level == LOG_INFO)
		m_File << "%%% INFO %%% ";
	else if (level == LOG_WARNING)
		m_File << "*** WARNING *** ";
	else
		m_File << "!!! ERROR !!! ";
	m_File << message <<std::endl;
	m_File.flush();
}