#include "Logger.h"

#include "termcolor.hpp"

#ifdef _DEBUG
Logger Logger::logger = Logger();
std::ofstream* Logger::logFile = nullptr;
#else
std::ofstream* Logger::logFile = new std::ofstream("log_" + Utils::currentDateTime("%m_%d_%Y_%H_%M_%S") + ".txt");
Logger Logger::logger = Logger(Logger::logFile);
#endif

Logger::Logger() : m_output(&std::cout) {
}

Logger::Logger(std::ostream* output) : m_output(output) {
	if (m_output->bad()) {
		LogAssert(false, "Invalid output.");
	}
}

Logger::~Logger() {
	m_output->flush();
	static std::stringstream closed_flag;
	m_output->rdbuf(closed_flag.rdbuf());
	if (logFile && *logFile) {
		logFile->close();
		delete logFile;
	}
}
	
static String replaceAll(const String& in, const String& what, const String& by) {
	String str = in;
	size_t index = 0;
	while (true) {
		index = str.find(what, index);
		if (index == std::string::npos) break;

		str.replace(what.size(), 3, by);

		index += what.size();
	}
	return str;
}

void Logger::print(LogLevel level, const char* file, const char* function, int line, const String& msg) {
	// [12/12/2017 23:45] => [ERROR] [func@33] Test error!
	String prefx = "[" + Utils::currentDateTime() + "] ";
	String filen = file;
	std::replace(filen.begin(), filen.end(), '\\', '/');
	filen = filen.substr(filen.find_last_of('/') + 1);

#ifdef _DEBUG
	(*m_output) << termcolor::green << termcolor::dark;
#endif

	(*m_output) << prefx;

#ifdef _DEBUG
	switch (level) {
		case LogLevel::Debug: (*m_output) << termcolor::cyan; break;
		case LogLevel::Info: (*m_output) << termcolor::blue; break;
		case LogLevel::Warning: (*m_output) << termcolor::yellow; break;
		case LogLevel::Error: (*m_output) << termcolor::red; break;
		case LogLevel::Fatal: (*m_output) << termcolor::magenta; break;
	}
#endif
	switch (level) {
		case LogLevel::Debug: (*m_output) << "[DBUG]"; break;
		case LogLevel::Info: (*m_output) << "[INFO]"; break;
		case LogLevel::Warning: (*m_output) << "[WARN]"; break;
		case LogLevel::Error: (*m_output) << "[ERRO]"; break;
		case LogLevel::Fatal: (*m_output) << "[FATL]"; break;
	}

#ifdef _DEBUG
	(*m_output) << termcolor::reset;
#endif

	(*m_output) << " [" << filen << "(" << function << " @ " << line << ")] " << msg << std::endl;
}
