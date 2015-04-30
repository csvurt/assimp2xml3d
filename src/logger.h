#include <iostream>

class Logger {
public:
	static enum Level { ERROR, WARN, INFO, DEBUG };
	static Level logLevel;

	static void Info(std::string msg);
	static void Warn(std::string msg);
	static void Debug(std::string msg);
	static void Error(std::string msg);
};