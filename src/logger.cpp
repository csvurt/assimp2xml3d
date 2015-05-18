#include "logger.h"

Logger::Level Logger::logLevel = Logger::WARN;

void Logger::Error(std::string msg) {
	std::cerr << "[Error] " << msg.c_str() << std::endl;
}

void Logger::Warn(std::string msg) {
	if (Logger::logLevel >= Logger::WARN) {
		std::cout << "[Warn] " << msg.c_str() << std::endl;
	}
}

void Logger::Info(std::string msg) {
	if (Logger::logLevel >= Logger::INFO) {
		std::cout << "[Info] " << msg.c_str() << std::endl;
	}
}

void Logger::Debug(std::string msg) {
	if (Logger::logLevel == Logger::DEBUG) {
		std::cout << "[Debug] " << msg.c_str() << std::endl;
	}
}

