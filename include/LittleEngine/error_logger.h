#pragma once
#include <iostream>
#include <sstream>
#include "Windows.h"


namespace LittleEngine
{

	inline void LogWarning(const std::string& msg)
	{
		std::string s = "LittleEngine WARNING: " + msg + "\n";
		std::cout << s;
	}

	inline void LogError(const std::string& msg)
	{
		std::string s = "LittleEngine ERROR: " + msg + "\n";

		std::cout << s;
		MessageBoxA(nullptr, s.c_str(), "ERROR", MB_OK | MB_ICONERROR);
	}


	inline void ThrowError(const std::string& msg)
	{
		std::string s = "LittleEngine CRITICAL ERROR: " + msg + "\n";

		std::cout << s;
		MessageBoxA(nullptr, s.c_str(), "CRITICAL ERROR", MB_OK | MB_ICONERROR);
		exit(1);
	}

}