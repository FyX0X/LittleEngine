#pragma once

#include <iostream>


namespace LittleEngine::Storage
{
	void EnsureDirectoryExists(const std::string& path);

	/**
	 * Returns the next available filename
	 * ex: 
	 * prefix = "test", ext = ".png"
	 * folder contains:
	 *		test0.png
	 *		test1.png
	 * 
	 * returns folder/test1.png
	 */
	std::string GetNextFreeFilepath(const std::string& folder, const std::string& prefix, const std::string& ext);
}