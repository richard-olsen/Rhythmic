#ifndef WGE_LOGGER_H_
#define WGE_LOGGER_H_

#include <wge_libs/spdlog/spdlog.h>

namespace WGE
{
	/*
	Initialize Logging
	*/
	void LoggerInit();

	std::shared_ptr<spdlog::logger> LoggerGetFileLog();
	std::shared_ptr<spdlog::logger> LoggerGetConsoleLog();
}


#ifdef WGE_DEBUG
#define LOG_TRACE(...) ::WGE::LoggerGetFileLog()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) {::WGE::LoggerGetFileLog()->debug(__VA_ARGS__); ::WGE::LoggerGetConsoleLog()->debug(__VA_ARGS__);}
#else
#define LOG_TRACE(...) {}
#define LOG_DEBUG(...) {}
#endif
#define LOG_INFO(...) {::WGE::LoggerGetFileLog()->info(__VA_ARGS__); ::WGE::LoggerGetConsoleLog()->info(__VA_ARGS__);}
#define LOG_WARNING(...) {::WGE::LoggerGetFileLog()->warn(__VA_ARGS__); ::WGE::LoggerGetConsoleLog()->warn(__VA_ARGS__);}
#define LOG_ERROR(...) {::WGE::LoggerGetFileLog()->error(__VA_ARGS__); ::WGE::LoggerGetConsoleLog()->error(__VA_ARGS__);}
#define LOG_CRITICAL(...) {::WGE::LoggerGetFileLog()->critical(__VA_ARGS__); ::WGE::LoggerGetConsoleLog()->critical(__VA_ARGS__);}

#endif
