#ifndef LOGGER_H
#define LOGGER_H

#include "Collections.h"
#include "Utils.h"
#include <fstream>
#include <iostream>

enum LogLevel {
	Debug = 0,
	Info,
	Warning,
	Error,
	Fatal
};

class Logger {
public:
	Logger();
	Logger(std::ostream* output);
	~Logger();

	void print(LogLevel level, const char* file, const char* function, int line, const String& msg);

	static Logger& getSingleton() { return logger; }
private:
	std::ostream* m_output;

	static Logger logger;
	static std::ofstream* logFile;
};

#define LOGGER Logger::getSingleton()

template<typename T>
String Str(const T& value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template<typename T, typename ... Args >
String Str(const T& value, const Args& ... args) {
	return Str(value) + Str(args...);
}

#ifdef __PRETTY_FUNCTION__
#define FUNCTION __PRETTY_FUNCTION__
#else
#define FUNCTION __FUNCTION__
#endif

#define Print(l, ...) LOGGER.print(l, __FILE__, FUNCTION, __LINE__, Utils::concat(__VA_ARGS__))
#define Log(...) Print(LogLevel::Debug, __VA_ARGS__)
#define LogInfo(...) Print(LogLevel::Info, __VA_ARGS__)
#define LogWarning(...) Print(LogLevel::Warning, __VA_ARGS__)
#define LogError(...) Print(LogLevel::Error, __VA_ARGS__)
#define LogFatal(...) Print(LogLevel::Fatal, __VA_ARGS__)
#define LogAssert(cond, ...) if (!(cond)) { Print(LogLevel::Fatal, __VA_ARGS__); __debugbreak(); }

#endif // LOGGER_H
