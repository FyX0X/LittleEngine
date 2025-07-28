#pragma once
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include "Windows.h"

#undef min
#undef max

#endif // _WIN32



namespace LittleEngine
{
	inline void ShowMessageBox(const std::string& title, const std::string& message)
	{

#ifdef _WIN32
		MessageBoxA(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);

#elif __APPLE__
		// macOS AppleScript popup
		std::string command = "osascript -e 'display dialog \"" + message + "\" with title \"" + title + "\" buttons {\"OK\"}'";
		system(command.c_str());

#elif __linux__
		// Try zenity, then kdialog, fallback to stderr
		if (system(("which zenity > /dev/null 2>&1")) == 0) {
			std::string command = "zenity --error --title=\"" + title + "\" --text=\"" + message + "\"";
			system(command.c_str());
		}
		else if (system(("which kdialog > /dev/null 2>&1")) == 0) {
			std::string command = "kdialog --error \"" + message + "\" --title \"" + title + "\"";
			system(command.c_str());
		}
		else {
			std::cerr << "ERROR: " << message << std::endl;
		}
#endif
	}

	inline void LogWarning(const std::string& msg)
	{
		std::string s = "LittleEngine WARNING: " + msg + "\n";
		std::cerr << s;
	}

	inline void LogError(const std::string& msg)
	{
		std::string s = "LittleEngine ERROR: " + msg + "\n";

		std::cerr << s;

		ShowMessageBox("LittleEngine ERROR", s);
	}


	inline void ThrowError(const std::string& msg)
	{
		std::string s = "LittleEngine CRITICAL ERROR: " + msg + "\n";

		std::cerr << s;

		ShowMessageBox("LittleEngine CRITICAL ERROR", s);

		exit(1);
	}

}