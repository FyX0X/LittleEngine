#include "LittleEngine/Utils/debug_tools.h"


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


#include <glad/glad.h>

#include <iostream>


namespace LittleEngine::Utils::DebugTools
{

	void InitializeDebugTools()
	{

		// Initialize call stack recording
#ifdef _WIN32
		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
		SymInitialize(GetCurrentProcess(), NULL, TRUE);
#endif

	}


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


	// Custom callbackFunction to pass to openGL for debugging
	void APIENTRY glDebugOutput(unsigned int source,
		unsigned int type,
		unsigned int id,
		unsigned int severity,
		int length,
		const char* message,
		const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
		} std::cout << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
		} std::cout << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
		} std::cout << std::endl;
		std::cout << std::endl;

		PrintStackTrace();

	}




}