#include <wge/core/logging.hpp>
#include <wge/io/filesystem.hpp>

#include <iostream>
#include <filesystem>

#include <wge_libs/spdlog/spdlog.h>
#include <wge_libs/spdlog/sinks/rotating_file_sink.h>
#include <wge_libs/spdlog/sinks/stdout_color_sinks.h>

namespace WGE
{
	std::shared_ptr<spdlog::logger> g_logger;
	std::shared_ptr<spdlog::logger> g_console;

	void LoggerInit()
	{
		spdlog::set_pattern("%^[%l] %n - [%T] [%t] - %v%$");
		
		std::string logs = FileSystem::GetFileInDirectory("/logs");
		if (!std::filesystem::is_directory(logs))
			std::filesystem::create_directory(logs);

		try
		{
			//g_logger = spdlog::daily_logger_mt("MainLog", logs + "/log");
			g_logger = spdlog::rotating_logger_mt("MainLog", logs + "/log", 1048576 * 5, 2);
			g_console = spdlog::stdout_color_mt("MainLog_console");
		}
		catch (const spdlog::spdlog_ex &ex)
		{
			#ifdef WIN32
			MessageBox(NULL, (std::string("Failed to initialize logger: ") + ex.what()).c_str(), "Error", MB_ICONERROR);
			#else
			std::cerr << "Failed to initialize logger: " << ex.what() << std::endl;
			#endif
			exit(1);
		}
		#ifdef WGE_DEBUG
		g_logger->set_level(spdlog::level::trace);
		g_console->set_level(spdlog::level::trace);
		#else
		g_logger->set_level(spdlog::level::info);
		g_console->set_level(spdlog::level::info);
		#endif

		g_logger->info("Logging Initialized WGE 0.0.1");
		g_console->info("Logging Initialized WGE 0.0.1");
	}

	std::shared_ptr<spdlog::logger> LoggerGetFileLog()
	{
		return g_logger;
	}
	std::shared_ptr<spdlog::logger> LoggerGetConsoleLog()
	{
		return g_console;
	}
}