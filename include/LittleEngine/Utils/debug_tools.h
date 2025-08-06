#pragma once

#include <string>

#ifndef APIENTRY
#define APIENTRY
#endif


namespace LittleEngine::Utils::DebugTools
{
	// Initialize the debug tools.
	// Sets up the stack trace
	void InitializeDebugTools();

	// Shows a message box with the given title and message.
	void ShowMessageBox(const std::string& title, const std::string& message);

	// Print a stack trace to the console.
	void PrintStackTrace();

	// Custom callbackFunction to pass to openGL for debugging
	void APIENTRY glDebugOutput(unsigned int source,
		unsigned int type,
		unsigned int id,
		unsigned int severity,
		int length,
		const char* message,
		const void* userParam);
}