#pragma once


#include <string>
#include <filesystem>
#include <fstream>


namespace LittleEngine::Utils
{


	enum class LogLevel
	{
		Info,
		Warning,
		Error,
		Critical
	};

	class Logger
	{
	public:



		static void Info(const std::string& message) { Log(LogLevel::Info, message); }
		static void Warning(const std::string& message) { Log(LogLevel::Warning, message); }
		static void Error(const std::string& message) { Log(LogLevel::Error, message); }
		static void Critical(const std::string& message);

		static void SetLogFile(const std::filesystem::path& filePath);
		static void CloseLogFile();


	private:
		static bool s_logToFile;
		static std::ofstream s_logFileStream;

		static void Log(LogLevel level, const std::string& message);

		static std::string GetTimestamp();


	};


}