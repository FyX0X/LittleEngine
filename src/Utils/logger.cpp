#include "LittleEngine/Utils/logger.h"


#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

#include "LittleEngine/Utils/file_system.h"
#include "LittleEngine/Utils/debug_tools.h"

#ifdef _WIN32
#include <windows.h>
#endif


namespace LittleEngine::Utils
{

	bool Logger::s_logToFile = false;
	std::ofstream Logger::s_logFileStream;



	void Logger::Critical(const std::string& message)
	{ 
		Log(LogLevel::Critical, message);

		DebugTools::PrintStackTrace();
		DebugTools::ShowMessageBox("Critical Error", message);

	}

	void Logger::SetLogFile(const std::filesystem::path& filePath)
	{

		if (s_logFileStream.is_open())
		{
			s_logFileStream.close();
		}
		if (!filePath.empty())
		{
			auto dir = filePath.parent_path();
			FileSystem::CreateDirectories(dir);

			s_logFileStream.open(filePath, std::ios::out | std::ios::app);
			if (!s_logFileStream.is_open())
			{
				std::cerr << "Failed to open log file: " << filePath.string();
				return;
			}
			s_logToFile = true;
		}
		else
		{
			s_logToFile = false;
		}
		
		Info("Logger initialized. Logging to file: " + filePath.string());

	}

	void Logger::CloseLogFile()
	{
		if (s_logFileStream.is_open())
		{
			s_logFileStream.close();
		}
		s_logToFile = false;
		Log(LogLevel::Info, "Log file cleared.");
	}

	void Logger::Log(LogLevel level, const std::string& message)
	{

		std::string levelStr;
		switch (level)
		{
		case LogLevel::Info:		levelStr = "[INFO]"; break;
		case LogLevel::Warning:		levelStr = "[WARNING]"; break;
		case LogLevel::Error:		levelStr = "[ERROR]"; break;
		case LogLevel::Critical:	levelStr = "[CRITICAL]"; break;
		default:					levelStr = "[UNKNOWN]"; break;
		}

		std::string logMessage = GetTimestamp() + " " + levelStr + " " + message;


#if DEVELOPMENT_BUILD == 1

		// Print to console with color

#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		WORD saved_attributes;

		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		saved_attributes = consoleInfo.wAttributes;

		switch (level)
		{
		case LogLevel::Info:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // white
			break;
		case LogLevel::Warning:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // yellow
			break;
		case LogLevel::Error:
		case LogLevel::Critical:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); // red
			break;
		default:
			SetConsoleTextAttribute(hConsole, saved_attributes);
			break;
		}
		std::cerr << logMessage << std::endl;
		SetConsoleTextAttribute(hConsole, saved_attributes);

#else 

			// ANSI escape codes for colors on *nix terminals
		switch (level)
		{
		case LogLevel::Info:
			std::cerr << "\033[37m"; // white		maybe use "\033[97m"; // bright white
			break;
		case LogLevel::Warning:
			std::cerr << "\033[33m"; // yellow
			break;
		case LogLevel::Error:
		case LogLevel::Critical:
			std::cerr << "\033[31m"; // red
			break;
		default:
			break;
		}
		std::cerr << logLine << "\033[0m";
#endif

#endif // 

		if (s_logToFile && s_logFileStream.is_open())
		{
			s_logFileStream << logMessage << std::endl;
		}


	}



	std::string Logger::GetTimestamp()
	{
		// 1. Build timestamp string
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm local_tm;
#ifdef _WIN32
		localtime_s(&local_tm, &now_c);
#else
		localtime_r(&now_c, &local_tm);
#endif
		std::ostringstream timestamp;
		timestamp << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
		return timestamp.str();
	}

} // namespace LittleEngine