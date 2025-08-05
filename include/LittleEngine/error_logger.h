#pragma once
#include <string>



namespace LittleEngine
{
	void ShowMessageBox(const std::string& title, const std::string& message);

	void LogWarning(const std::string& msg);

	void LogError(const std::string& msg);


	void ThrowError(const std::string& msg);

	void PrintStackTrace();

}