#include "LittleEngine/error_logger.h"


#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#include <unistd.h>
#include <cxxabi.h>
#include <cstdlib>
#endif


#include <fstream>
#include <cstdlib>
#include <iostream>


namespace LittleEngine
{
	

	void ShowMessageBox(const std::string& title, const std::string& message)
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

	void LogWarning(const std::string& msg)
	{
		std::string s = "LittleEngine WARNING: " + msg + "\n";
		std::cerr << s;
	}

	void LogError(const std::string& msg)
	{
		std::string s = "LittleEngine ERROR: " + msg + "\n";

		std::cerr << s;

		ShowMessageBox("LittleEngine ERROR", s);
	}


	void ThrowError(const std::string& msg)
	{
		std::string s = "LittleEngine CRITICAL ERROR: " + msg + "\n";

		std::cerr << s;

		ShowMessageBox("LittleEngine CRITICAL ERROR", s);

		exit(1);
	}

	void PrintStackTrace()
	{
		constexpr int MAX_FRAMES = 64;

#if defined(_WIN32)
		void* stack[MAX_FRAMES];
		HANDLE process = GetCurrentProcess();

		USHORT frames = CaptureStackBackTrace(0, MAX_FRAMES, stack, nullptr);

		SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256, 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		SymInitialize(process, NULL, TRUE);

		std::cerr << "Call stack:\n";
		for (USHORT i = 0; i < frames; ++i) {
			if (SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol)) {
				std::cerr << i << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << "\n";
			}
		}

		free(symbol);

#else
		void* buffer[MAX_FRAMES];
		int nptrs = backtrace(buffer, MAX_FRAMES);
		char** symbols = backtrace_symbols(buffer, nptrs);

		std::cerr << "Call stack:\n";
		for (int i = 0; i < nptrs; ++i) {
			// Try to demangle
			char* mangledName = symbols[i];
			char* demangled = nullptr;
			int status = 0;

			// attempt to demangle
			demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
			if (status == 0 && demangled) {
				std::cerr << i << ": " << demangled << "\n";
				free(demangled);
			}
			else {
				std::cerr << i << ": " << symbols[i] << "\n";
			}
		}

		free(symbols);
#endif
	}



} // namespace LittleEngine